import subprocess;
import shutil;
import os;
from pathlib import Path
import argparse
import sys

CMAKE_ARGS = ["-DGLFW_BUILD_EXAMPLES=OFF", "-DGLFW_BUILD_TESTS=OFF", "-DGLFW_BUILD_DOCS=OFF", "-DGLFW_INSTALL=OFF"]


def find_project_root():
    current = Path(__file__).parent
    while current != current.parent:
        if (current / "CMakeLists.txt").exists():
            return current
        current = current.parent
    raise RuntimeError("Could not find project root")


def start_cmake(libraryType, isDist):
    extraCmakeArgs = []
    if libraryType == "shared":
        extraCmakeArgs.append("-DFLUX_BUILD_SHARED=ON")
    else:
        extraCmakeArgs.append("-DFLUX_BUILD_SHARED=OFF")

    if isDist:
        extraCmakeArgs.append("-DFLUX_BOOTSTRAPPER_PUBLISH=ON")

    subprocess.run(["cmake", "-B", "build", "-A", "x64"] + extraCmakeArgs + CMAKE_ARGS, check=True)


def build_engine(config):
    print("Building Flux Engine")
    subprocess.run(["cmake", "--build", "build", "--target", 
                    "flux-runtime", "--config", config], check=True)
    

def build_host(config):
    print("Building Flux Host")
    subprocess.run(["cmake", "--build", "build", "--target", 
                    "flux-host", "--config", config], check=True)
    

def build_bootstrapper(config):
    print("Building Flux Bootstrapper")
    subprocess.run(["cmake", "--build", "build", "--target",
                    "flux-bootstrapper", "--config", config], check=True)
    

def package_dist(libraryType):
    print("Packaging into dist")
    releasePath = Path("build/bin/Release")
    distPath = Path("build/dist")

    Path(distPath).mkdir(parents=True, exist_ok=True)
    Path(distPath / "Bin").mkdir(parents=True, exist_ok=True)
    Path(distPath / "TemplateFiles").mkdir(parents=True, exist_ok=True)

    exeExt = ""
    sharedLibExt = ""

    if sys.platform == "win32":
        exeExt = ".exe"
        sharedLibExt = ".dll"
    elif sys.platform == "darwin":
        sharedLibExt = ".dylib"
    elif sys.platform.startsWith("linux"):
        sharedLibExt = ".so"

    shutil.copy(releasePath / f"flux{exeExt}", distPath / f"flux{exeExt}")
    shutil.copytree(releasePath / "TemplateFiles", distPath / "TemplateFiles", dirs_exist_ok=True)
    shutil.copy(releasePath / f"flux-host{exeExt}", distPath / "Bin" / f"flux-host{exeExt}")
    shutil.copy(releasePath / "Flux.ScriptCore.dll", distPath / "Bin" / "Flux.ScriptCore.dll")
    shutil.copy(releasePath / f"mono-2.0-sgen{sharedLibExt}", distPath / "Bin" / f"mono-2.0-sgen{sharedLibExt}")

    if libraryType == "shared":
        shutil.copy(releasePath / f"flux-runtime{sharedLibExt}", distPath / "Bin" / f"flux-runtime{sharedLibExt}")
    
    
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
        default="Release",
        help="Specify the build configuration (Release or Debug)"
    )

    parser.add_argument(
        "-l", "--library",
        choices=["static", "shared"],
        required=False,
        default="static",
        help="Specify what type of library the flux runtime should be built as (static or shared)"
    )

    parser.add_argument(
        "-d", "--dist",
        action="store_true",
        help="Create a distrubition directory"
    )

    parser.add_argument(
        "-fh", "--flux-host",
        action="store_true",
        help="Build the flux-host target"
    )

    parser.add_argument(
        "-fb", "--flux-bootstrapper",
        action="store_true",
        help="Build the flux-bootstrapper target"    
    )

    args = parser.parse_args()
    project_root = find_project_root()
    os.chdir(project_root)

    if args.dist:
        args.flux_host = True
        args.flux_bootstrapper = True

    start_cmake(args.library, args.dist)
    build_engine(args.configuration)

    if args.flux_host:
        build_host(args.configuration)

    if args.flux_bootstrapper:
        build_bootstrapper(args.configuration)

    if args.dist:
        package_dist(args.library)