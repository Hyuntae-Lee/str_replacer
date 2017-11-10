use std::env;
use std::fs::File;
use std::io::Read;

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

    // file read
    // - open
    let mut contents = String::new();
    let mut file = match File::open(tar_file_path) {
        Err(r) => {
            println!("{:?}", r);
            return;
        },
        Ok(x) => x
    };
    // - read
    match file.read_to_string(&mut contents) {
        Err(r) => {
            println!("{:?}", r);
            return;
        },
        Ok(x) => x
    };

    // test
    println!("{}", contents);
}

fn parse_input<'a>(input : &'a Vec<String>) -> Result<(&'a str, &'a str, &'a str), &'a str> {
    if input.capacity() != 4 {
        return Err("Two few input!");
    }

    return Ok((&input[1], &input[2], &input[3]));
}
