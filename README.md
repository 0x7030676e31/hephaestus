# Hephaestus

**Hephaestus** is a proof-of-concept modular execution framework developed to explore techniques for bypassing certain Endpoint Detection and Response (EDR) systems and antivirus (AV) solutions. It enables the execution of arbitrary Rust code within a Python process by utilizing a C-to-Python binding and compiling the Rust code into a DLL.

The framework operates by creating a suspended instance of `notepad.exe`, injecting the compiled DLL into its thread, and executing the payload. An included installation script assists with setting up the runtime environment, installing required components, and configuring registry keys for automated startup.

---

## ⚙️ Execution Instructions

Follow the steps below to build and execute the framework:

### 1. Build the Rust Code

From the project root directory, compile the Rust source code:

```bash
cargo build --release
```

This will generate a DLL file located in the `target/release` directory.

### 2. Build the Python C Extension

Build the C extension for Python using the following command:

```bash
python3 setup.py build_ext --inplace
```

This will produce a `.pyd` file (a Python dynamic library) in the project directory.

### 3. Prepare the Execution Directory

Move the necessary files to the `execution` directory:

- Copy the compiled DLL from `target/release` to `execution/.`
- Copy the `.pyd` file from the build output to `execution/.`

Ensure the following files are present in the `execution` directory (these may already be provided):

- `hephaestus.dll`
- `injector.cp312-win_amd64.pyd`
- `main.py`

### 4. Run the Python Script

Execute the Python script from within the `execution` directory:

```bash
python main.py
```

This will:

- Create a suspended instance of `notepad.exe`
- Inject the compiled DLL into its thread
- Execute the Rust payload

> ⚠️ By default, the Rust code displays a MessageBox. You may modify the Rust source to implement custom functionality as required.

## 🛠️ Installation Script Overview

A PowerShell script is included to automate setup, dependency retrieval, and persistence configuration.

### Key Features

- **Environment Setup**
Creates a clean working directory under `%LOCALAPPDATA%` and removes any existing files.

- **Python Installation**
Installs Python 3.12.8 silently if not already present in the expected location.

- **File Download**
Fetches the required files (`client.pyw`, `.pyd` module, and `core.dll`) from a remote server.

- **Persistence**
Adds a registry key under `HKCU\Software\Microsoft\Windows\CurrentVersion\Run` to launch the script at user login.

- **Execution & Cleanup**
Starts the script using pythonw.exe and deletes PowerShell history and recent run entries to reduce forensic traces.

> ⚠️ Disclaimer: This script is provided for educational and research purposes only. Unauthorized use on systems without explicit permission is strictly prohibited and may violate applicable laws.
