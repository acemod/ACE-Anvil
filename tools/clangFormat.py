import os
import sys
import subprocess
import concurrent.futures
import tomllib

addons_base_path = os.path.join(os.path.dirname(os.path.dirname(os.path.realpath(__file__))), "addons")

# todo: check path or llvm but this works for now if you have vs
clang_tidy_exe = os.path.join(r"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\Llvm\x64\bin\clang-format.exe")

def process():
    for root, _dirs, files in os.walk(addons_base_path):
        for file in files:
            if file.endswith(".c"):
                filePath = os.path.join(root, file)
                fakePath = filePath + "s"
                print(f"proccsing {filePath}")

                os.rename(filePath, fakePath)

                cmd = [clang_tidy_exe, "-i", fakePath]
                ret = subprocess.call(cmd)

                os.rename(fakePath, filePath)
    return 0


def main():
    if not os.path.isfile(clang_tidy_exe):
        print(f"Error: not found {clang_tidy_exe}")
        return 1
    return process()

if __name__ == "__main__":
    sys.exit(main())
