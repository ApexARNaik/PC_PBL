# Smart Parking Allocation System - Setup Guide

This guide provides detailed, step-by-step instructions on how to install a C compiler on Windows, build this project, and run it.

Because C code is "compiled," your computer needs a compiler tool (like `gcc`) to translate the human-readable `.c` files into a machine-readable `.exe` file before it can be run.

---

## Step 1: Install a C Compiler on Windows

Since you are using Windows, the easiest and most standard way to get `gcc` (the GNU C Compiler) is to use **MSYS2 / MinGW-w64**.

### Option A: Install via MSYS2 (Recommended)

1. Go to the MSYS2 website: [https://www.msys2.org/](https://www.msys2.org/)
2. Download the installer (e.g., `msys2-x86_64-xxxxxxxx.exe`) and run it. Keep all default settings and click **Next** until it finishes.
3. Once finished, an MSYS2 terminal window will pop up.
4. In that terminal, type the following command and press Enter to install the GCC compiler and Make tools:
   ```bash
   pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-make
   ```
5. It will ask you to confirm. Type `Y` and press Enter. Let it download and install.

### Add GCC to your System PATH

For PowerShell or Command Prompt to recognize the `gcc` command, you need to add it to your Windows Environment Variables.

1. Press the Windows key, type **"Environment Variables"**, and select **"Edit the system environment variables"**.
2. Click the **"Environment Variables..."** button at the bottom right.
3. Under **"System variables"** (the bottom list), find the variable named **`Path`**, click it, and click **"Edit..."**.
4. Click **"New"** and paste the exact path to your newly installed compiler's `bin` folder. If you kept the defaults, it will be:
   `C:\msys64\ucrt64\bin`
5. Click **OK** on all three windows to save.
6. **IMPORTANT:** Close any open PowerShell or Command Prompt windows and open a new one so it recognizes the updated Path.

### Verify the Installation

Open a new PowerShell terminal and run:

```bash
gcc --version
```

If you see text describing the GCC version, you are good to go!

---

## Step 2: Compile the Project

Now that you have a compiler, you need to navigate to your project folder and compile the code.

1. Open PowerShell and navigate to the project directory:

   ```powershell
   cd "Downloads\PC_PBL"
   ```

2. **Compile using the manual command**:
   Run the following command to link all the `.c` files together and generate the final `parking_system.exe`:
   ```bash
   gcc main.c slots.c allocation.c billing.c report.c storage.c -o parking_system
   ```

_(Alternatively, if you installed `make` successfully, you can run `mingw32-make` or `make` depending on your alias, but the `gcc` command above is foolproof)._

---

## Step 3: Run the Program

Once the compilation finishes successfully (it shouldn't print any errors), the `.exe` file will be created.

To start your smart parking backend, run:

```powershell
.\parking_system.exe
```

---

## Step 4: How to Use the App

1. **Option 1 (Park a Vehicle):** Type `1` and press Enter. Choose a vehicle type (e.g., `1` for Car) and type a license plate (e.g., `MH12AB1234`). The system will assign a slot.
2. **Option 3 (View Occupancy):** Type `3` to see a table of all 20 slots and who is parked where.
3. **Option 2 (Remove Vehicle):** Type `2` and enter the license plate to simulate the vehicle leaving. It will calculate the fee based on the time spent and free up the slot.
4. **Option 6 (Exit):** Close the program. The current layout is saved securely to `slots_data.txt` so it will remember where cars are parked when you open it next time!

---

## Troubleshooting

- **"gcc is not recognized..."**: This means Step 1 (Adding to PATH) failed. Make sure you entered the correct `bin` folder path (`C:\msys64\ucrt64\bin`) and that you completely closed and reopened your terminal.
- **"Permission denied" when saving files**: Ensure the folder where the project is located is not set to Read-Only and that you aren't running it from a zipped folder.
