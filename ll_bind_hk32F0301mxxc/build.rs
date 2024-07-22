use std::path::PathBuf;
use bindgen;

fn main() {
    let mut cc_build = cc::Build::new();
    cc_build.target("thumbv6m-none-eabi");

    let mut c_files = Vec::new();
    let mut h_files = Vec::new();
    let mut h_floders = Vec::new();

    for entry in walkdir::WalkDir::new("./csrc") {
        if let Ok(entry) = entry {
            if let Some(ext) = entry.path().extension() {
                if ext == "c" {
                    c_files.push(entry.path().to_str().unwrap().to_string());
                }
                if ext == "h" {
                    h_files.push(entry.path().to_str().unwrap().to_string());
                    let h_path = entry.path();
                    let h_floder = h_path.parent().unwrap().to_str().unwrap().to_string();
                    if !h_floders.contains(&h_floder.to_string()) {
                        h_floders.push(h_floder.to_string());
                    }
                }
            }
        }
    }
    cc_build.includes(h_floders);
    cc_build.files(&c_files);
    
    cc_build.define("USE_FULL_ASSERT", None);

    cc_build.opt_level(2);

    cc_build.compile("arm-none-eabi-gcc");

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
