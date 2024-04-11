import os
import shutil
import sys
import subprocess
import concurrent.futures

addons_base_path = os.path.join(os.path.dirname(os.path.dirname(os.path.realpath(__file__))), "addons")

clang_tidy_exe = exe = shutil.which("clang-format") # https://github.com/llvm/llvm-project/releases
if clang_tidy_exe is None: clang_tidy_exe = os.path.join(r"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\Llvm\x64\bin\clang-format.exe")

def process_file(filePath):
    fakePath = filePath + "s"
    os.rename(filePath, fakePath)
    ret = subprocess.call([clang_tidy_exe, "-i", fakePath])
    os.rename(fakePath, filePath)
    return [filePath, ret]

def main():
    if not os.path.isfile(clang_tidy_exe):
        print(f"Error: not found {clang_tidy_exe}")
        return 1
    
    arma_files = []
    for root, _dirs, files in os.walk(addons_base_path):
        for file in files:
            if file.endswith(".c"): 
                arma_files.append(os.path.join(root, file))

    print("clanging {} .c arma files".format(len(arma_files)))
    with concurrent.futures.ThreadPoolExecutor(max_workers=24) as executor:
        for filePath, ret in executor.map(process_file, arma_files):
            if ret != 0:
                print(f"error {ret} with {filePath}")

    return 0

if __name__ == "__main__":
    sys.exit(main())
