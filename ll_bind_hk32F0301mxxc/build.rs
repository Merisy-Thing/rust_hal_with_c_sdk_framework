use std::path::PathBuf;
use bindgen;

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
    
    cc_build.define("USE_FULL_ASSERT", None);

    #[cfg(feature = "gcc")]
    {
        cc_build.target("thumbv6m-none-eabi");
        cc_build.opt_level(2);
        cc_build.compiler("arm-none-eabi-gcc");
    }
    #[cfg(feature = "clang")]
    {
        cc_build.target("armv6m-none-eabi");
        cc_build.opt_level_str("s");
        cc_build.compiler("clang");
        cc_build.archiver("llvm-ar");
    }

    cc_build.compile("ll_bind_hk32F0301mxxc");

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
}
