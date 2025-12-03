# Use GCC image
FROM gcc:latest

WORKDIR /usr/src/app

COPY . .

# Compile the code using the existing makefile
RUN make

CMD ["./stego.exe"]
