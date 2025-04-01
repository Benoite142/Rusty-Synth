# Synthetiser

## Project Setup

### CPP Dependencies

Two main libraries are needed to compile the program:
- libasound2-dev 
- libx11-dev
- libboost-all-dev

As for the tools we use, you will need `make`, alongside a valid CPP compiler (we use g++ with std-c++20).

### Front-End Dependencies

The front-end app runs on electron, which requires you to have `npm`.
All the dependencies (including dev dependencies) should be installable through `npm install` in the `interface` directory.
You might need to update your `Nodejs` version as some dependencies require recent versions.
To help manage your versions you should consider using `nvm`.
