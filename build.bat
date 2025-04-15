@echo off

cargo build --release
python setup.py build_ext --inplace

copy /Y target\release\hephaestus.dll execution\hephaestus.dll
copy /Y injector.cp313-win_amd64.pyd execution\injector.cp313-win_amd64.pyd

del target\release\hephaestus.dll
del injector.cp313-win_amd64.pyd

echo:
echo ===========================================================
echo To inject the Rust DLL, launch the 'main.py' python script.
echo ===========================================================
