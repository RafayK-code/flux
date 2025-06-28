import subprocess;
import shutil;
import os;
from pathlib import Path
from enum import Enum
import argparse

CMAKE_ARGS = ["-DGLFW_BUILD_EXAMPLES=OFF", "-DGLFW_BUILD_TESTS=OFF", "-DGLFW_BUILD_DOCS=OFF", "-DGLFW_INSTALL=OFF"]


def find_project_root():
    current = Path(__file__).parent
    while current != current.parent:
        if (current / "CMakeLists.txt").exists():
            return current
        current = current.parent
    raise RuntimeError("Could not find project root")


def build_engine(config, libraryType):
    print("Building Flux")

    if libraryType == "shared":
        libraryCmakeConfig = "-DCMAKE_FLUX_BUILD_SHARED=ON"
    else:
        libraryCmakeConfig = "-DCMAKE_FLUX_BUILD_SHARED=OFF"

    subprocess.run(["cmake", "-B", "build", "-A", "x64", libraryCmakeConfig] + CMAKE_ARGS, check=True)
    subprocess.run(["cmake", "--build", "build", "--target", 
                    "flux-bootstrapper", "--config", config], check=True)
    
    
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Build configuration parser")

    parser.add_argument(
        "-g", "--generator",
        choices=["vs", "ninja"],
        required=True,
        help="Specify the build system generator (vs or ninja)"
    )

    parser.add_argument(
        "-c", "--configuration",
        choices=["Release", "Debug"],
        required=False,
        default="Debug",
        help="Specify the build configuration (Release or Debug)"
    )

    parser.add_argument(
        "-l", "--library",
        choices=["static", "shared"],
        required=False,
        default="static",
        help="Specify what type of library the flux runtime should be built as (static or shared)"
    )

    args = parser.parse_args()
    project_root = find_project_root()
    os.chdir(project_root)

    build_engine(args.configuration, args.library)