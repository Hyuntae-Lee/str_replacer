use std::env;
use std::fs::File;
use std::io::{Read, Write};

fn main() {
    // parse arguments
    let args: Vec<String> = env::args().collect();
    let (tar_file_path, str_to_replace, str_to_replace_with) = match parse_input(&args) {
        Err(r) => {
            println!("{:?}", r);
            return;
        },
        Ok(x) => x
    };

    // read
    let mut contents = String::new();
    match read_contents(&mut contents, tar_file_path) {
        Err(r) => {
            println!("{:?}", r);
            return;
        },
        Ok(size) => {
            if size == 0 {
                println!("File is empty!");
                return;
            }
        }
    };

    // replace string
    let new_contents = contents.replace(str_to_replace, str_to_replace_with);

    // write to file
    write_contents(tar_file_path, new_contents.as_str());
}

fn parse_input<'a>(input : &'a Vec<String>) -> Result<(&'a str, &'a str, &'a str), &'a str> {
    if input.capacity() != 4 {
        return Err("Two few input!");
    }

    return Ok((&input[1], &input[2], &input[3]));
}

fn read_contents(contents : &mut String, file_path : &str) -> Result<usize, String> {
    // open
    let mut file = match File::open(file_path) {
        Err(r) => {
            return Err(format!("{:?}", r));
        },
        Ok(x) => x
    };
    // read
    let contents_size = match file.read_to_string(contents) {
        Err(r) => {
            return Err(format!("{:?}", r));
        },
        Ok(x) => x
    };

    return Ok(contents_size);
}

fn write_contents<'a>(file_path : &str, contents : &str) {
    // open
    let mut file = match File::create(file_path) {
        Err(r) => {
            return println!("{:?}", r);
        },
        Ok(x) => x
    };
    // write
    file.write_all(contents.as_bytes());
}
