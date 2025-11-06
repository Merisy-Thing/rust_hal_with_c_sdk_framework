use std::path::PathBuf;
use bindgen;
use std::collections::HashSet;
use std::env;

fn main() {
    let mut cc_build = cc::Build::new();
    let mut c_files = Vec::new();
    let mut h_files = Vec::new();
    let mut h_folders = Vec::new();

    for entry in walkdir::WalkDir::new("./csrc") {
        if let Ok(entry) = entry {
            if let Some(ext) = entry.path().extension() {
                if ext == "c" {
                    c_files.push(entry.path().to_str().unwrap().to_string());
                }
                if ext == "h" {
                    h_files.push(entry.path().to_str().unwrap().to_string());
                    let h_path = entry.path();
                    let h_folder = h_path.parent().unwrap().to_str().unwrap().to_string();
                    if !h_folders.contains(&h_folder.to_string()) {
                        h_folders.push(h_folder.to_string());
                    }
                }
            }
        }
    }
    cc_build.includes(h_folders);
    cc_build.files(&c_files);
    
    cc_build.compiler("riscv-none-embed-gcc");
	
    cc_build.file("csrc\\c_sdk_lib\\SRC\\Startup\\startup_ch32v20x_D6.S");

    cc_build.opt_level(2);
    cc_build.flag("-march=rv32imacxw").flag("-mabi=ilp32").flag("-msmall-data-limit=8");
    //cc_build.asm_flag("-x assembler-with-cpp");

    cc_build.compile("ll_bind_ch32v20x");

    //bindgen ll_drv
    let bindings = bindgen::Builder::default()
            .header("csrc/wrapper.h")
            .use_core()
            .parse_callbacks(Box::new(bindgen::CargoCallbacks::new()));
    if let Ok(bind_result) = bindings.generate() {
        let out_path = PathBuf::from("./bind");
        bind_result
            .write_to_file(out_path.join("bind_ll_drv.rs"))
            .expect("Couldn't write bindings!");
    } else {
        panic!("Error: Unable to generate bind_ll_drv");
    }

    for c in c_files {
        if !c.contains("c_sdk_lib") {
            println!("{}", &format!("cargo:rerun-if-changed={c}"));
        }
    }
    for h in h_files {
        if !h.contains("c_sdk_lib") {
            println!("{}", &format!("cargo:rerun-if-changed={h}"));
        }
    }

    rt_main();
}

fn rt_main() {
    let target = env::var("TARGET").unwrap();
    let cargo_flags = env::var("CARGO_ENCODED_RUSTFLAGS").unwrap();
    // set configuration flags depending on the target
    if target.starts_with("riscv") {
        println!("cargo:rustc-cfg=riscv");

        // This is required until target_arch & target_feature risc-v work is
        // stable and in-use (rust 1.75.0)
        let (_bits, extensions) = parse_target(&target, &cargo_flags);

        // expose the ISA extensions
        for ext in &extensions {
            println!("cargo:rustc-cfg=riscv{}", ext);
        }
    }
}

/// Parse the target RISC-V architecture and returns its bit width and the extension set
fn parse_target(target: &str, cargo_flags: &str) -> (u32, HashSet<char>) {
    // isolate bit width and extensions from the rest of the target information
    let arch = target
        .trim_start_matches("riscv")
        .split('-')
        .next()
        .unwrap();

    let bits = arch
        .chars()
        .take_while(|c| c.is_ascii_digit())
        .collect::<String>()
        .parse::<u32>()
        .unwrap();

    let mut extensions: HashSet<char> = arch.chars().skip_while(|c| c.is_ascii_digit()).collect();
    // expand the 'g' shorthand extension
    if extensions.contains(&'g') {
        extensions.insert('i');
        extensions.insert('m');
        extensions.insert('a');
        extensions.insert('f');
        extensions.insert('d');
    }

    let cargo_flags = cargo_flags
        .split(0x1fu8 as char)
        .filter(|arg| !arg.is_empty());

    cargo_flags
        .filter(|k| k.starts_with("target-feature="))
        .flat_map(|str| {
            let flags = str.split('=').collect::<Vec<&str>>()[1];
            flags.split(',')
        })
        .for_each(|feature| {
            let chars = feature.chars().collect::<Vec<char>>();
            match chars[0] {
                '+' => {
                    extensions.insert(chars[1]);
                }
                '-' => {
                    extensions.remove(&chars[1]);
                }
                _ => {
                    panic!("Unsupported target feature operation");
                }
            }
        });

    (bits, extensions)
}