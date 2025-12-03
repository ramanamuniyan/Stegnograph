# Stegnograph
# BMP Steganography (Encode & Decode)

This project implements **image steganography** in C using the **Least Significant Bit (LSB)** technique.  
You can **encode a secret file** (any type: text, pdf, image, etc.) into a `.bmp` image and later **decode it** back.

---

## 📌 Features
- Encode any secret file into a 24-bit BMP image
- Decode and extract the hidden file from a stego BMP
- Supports secret file extension detection and auto-reconstruction
- Preserves the BMP header and pixel data integrity
- Error handling for file operations and capacity checks

---

## 🛠️ Requirements
- GCC (or any C compiler)
- Linux / Ubuntu (tested)
- 24-bit BMP image as source cover file

---

## 📂 Project Structure
```
├── encode.c        # Encoding functions
├── decode.c        # Decoding functions
├── encode.h        # Encode header file
├── decode.h        # Decode header file
├── types.h         # Common data structures and enums
├── main.c          # Entry point
├── Makefile        # To build project
└── README.md       # Documentation
```

---

## ⚙️ Build Instructions

Clone this repository and build:

```bash
git clone https://github.com/<your-username>/<repo-name>.git
cd <repo-name>
make
```

This will generate the executable:
- `stego.exe`

---

## ▶️ Usage

### Encoding
Hide a secret file inside a BMP image.

```bash
./stego.exe -e <source.bmp> <secret_file> <output_stego.bmp>
```

Example:
```bash
./stego.exe -e input.bmp secret.txt stego.bmp
```

**Note:** You will be prompted to enter a "magic string" (password) during execution.

---

### Decoding
Extract the secret file from the stego BMP.

```bash
./stego.exe -d <stego.bmp>
```

Example:
```bash
./stego.exe -d stego.bmp
```

**Note:** You must enter the same "magic string" used during encoding.
This will create a file named `decode_file.<ext>` where `<ext>` is the original extension of the hidden file.

---

## 🐳 Docker Usage

You can run this application using Docker without installing GCC or other dependencies on your machine.

### 1. Build the Docker Image
Run this command in the root of the project:

```bash
docker build -t stego-app .
```

### 2. Run with Docker

To interact with files on your local machine (read input, save output), we use volume mounting (`-v`).

**Encoding:**
```bash
docker run -it --rm -v "$(pwd):/usr/src/app" stego-app ./stego.exe -e <source.bmp> <secret_file> <output.bmp>
```
*Example:*
```bash
docker run -it --rm -v "$(pwd):/usr/src/app" stego-app ./stego.exe -e sample.bmp secret.txt encoded.bmp
```

**Decoding:**
```bash
docker run -it --rm -v "$(pwd):/usr/src/app" stego-app ./stego.exe -d <stego.bmp>
```
*Example:*
```bash
docker run -it --rm -v "$(pwd):/usr/src/app" stego-app ./stego.exe -d encoded.bmp
```

**Command Breakdown:**
- `-it`: Interactive mode (required for entering the magic string).
- `--rm`: Remove the container after it finishes.
- `-v "$(pwd):/usr/src/app"`: Mount current directory to container so it can access your files.
- `stego-app`: The name of the image you built.

---

## 🧪 Example Workflow

```bash
# Encode
./stego.exe -e lena.bmp notes.txt stego.bmp
# Prompts: Enter the magic string: [your-password]

# Decode
./stego.exe -d stego.bmp
# Prompts: Enter the magic string to decode: [your-password]
# Output → decode_file.txt
```

---

## 🚧 Limitations
- Works only with **24-bit BMP images**
- The cover image must be large enough to hold the secret file
- Currently assumes header size = 54 bytes (standard BMP); some BMP variants may differ

---



---

## 👨‍💻 Author
- Ramana

---

## 📑 Header Files

### encode.h
- Contains the `EncodeInfo` structure used during encoding
- Defines constants for buffer sizes and ANSI color codes:
  - `RESET`, `RED`, `GREEN` (for colored console messages)
- Declares all encoding-related functions such as:
  - `check_operation_type()`
  - `read_and_validate_encode_args()`
  - `do_encoding()`
  - `open_files()`, `check_capacity()`
  - `encode_magic_string()`, `encode_secret_file_extn()`, `encode_secret_file_data()`
  - `encode_byte_to_lsb()`, `encode_size_to_lsb()`
  - `copy_remaining_img_data()`
  - 
