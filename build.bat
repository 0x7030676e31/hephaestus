@echo off

cargo build --release
move target\release\hephaestus.dll hephaestus.dll
python3 setup.py build_ext --inplace

echo ===========================================================
echo To inject the Rust DLL, launch the 'main.py' python script.
echo ===========================================================
