import subprocess;
import shutil;
import os;
from pathlib import Path
import argparse
import sys
from dataclasses import dataclass

CMAKE_ARGS = ["-DGLFW_BUILD_EXAMPLES=OFF", "-DGLFW_BUILD_TESTS=OFF", "-DGLFW_BUILD_DOCS=OFF", "-DGLFW_INSTALL=OFF"]

@dataclass
class CmakeConfig:
    libraryType: str
    isDist: bool
    cppDemo: bool
    boostrappedDemo: bool


def find_project_root():
    current = Path(__file__).parent
    while current != current.parent:
        if (current / "CMakeLists.txt").exists():
            return current
        current = current.parent
    raise RuntimeError("Could not find project root")


def start_cmake(config: CmakeConfig):
    extraCmakeArgs = []
    if config.libraryType == "shared":
        extraCmakeArgs.append("-DFLUX_BUILD_SHARED=ON")
    else:
        extraCmakeArgs.append("-DFLUX_BUILD_SHARED=OFF")

    if config.isDist:
        extraCmakeArgs.append("-DFLUX_BOOTSTRAPPER_PUBLISH=ON")
    else:
        extraCmakeArgs.append("-DFLUX_BOOTSTRAPPER_PUBLISH=OFF")

    if config.cppDemo:
        extraCmakeArgs.append("-DFLUX_DEMO_CPP=ON")
    else:
        extraCmakeArgs.append("-DFLUX_DEMO_CPP=OFF")

    if config.boostrappedDemo:
        extraCmakeArgs.append("-DFLUX_DEMO_BOOTSTRAPPED=ON")
    else:
        extraCmakeArgs.append("-DFLUX_DEMO_BOOTSTRAPPED=OFF")

    subprocess.run(["cmake", "-B", "build", "-A", "x64"] + extraCmakeArgs + CMAKE_ARGS, check=True)


def build_engine(config):
    print("Building Flux Engine")
    subprocess.run(["cmake", "--build", "build", "--target", 
                    "flux", "--config", config], check=True)
    

def build_runtime(config):
    print("Building Flux Runtime")
    subprocess.run(["cmake", "--build", "build", "--target", 
                    "flux-runtime", "--config", config], check=True)
    

def build_bootstrapper(config):
    print("Building Flux Bootstrapper")
    subprocess.run(["cmake", "--build", "build", "--target",
                    "flux-bootstrapper", "--config", config], check=True)
    

def build_demo_cpp(config):
    print("Building Flux Cpp Demo")
    subprocess.run(["cmake", "--build", "build", "--target",
                    "flux-demo-cpp", "--config", config], check=True)
    

def build_demo_bootstrapped(config):
    print("Building Flux Cpp Demo")
    subprocess.run(["cmake", "--build", "build", "--target",
                    "flux-demo-bootstrapped", "--config", config], check=True)
    

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


def create_arg_parser():
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
        help="Specify what type of library the flux engine should be built as (static or shared)"
    )

    parser.add_argument(
        "-d", "--dist",
        action="store_true",
        help="Create a distrubition directory"
    )

    parser.add_argument(
        "-fr", "--flux-runtime",
        action="store_true",
        help="Build the flux-runtime target"
    )

    parser.add_argument(
        "-fb", "--flux-bootstrapper",
        action="store_true",
        help="Build the flux-bootstrapper target"    
    )

    parser.add_argument(
        "-fd", "--flux-demo",
        action="store_true",
        help="Build both demo targets"
    )

    parser.add_argument(
        "-fdc", "--flux-demo-cpp",
        action="store_true",
        help="Build the demo-cpp target"
    )

    parser.add_argument(
        "-fdb", "--flux-demo-bootstrapped",
        action="store_true",
        help="Build the demo-bootstrapped target"
    )

    return parser


if __name__ == "__main__":
    parser = create_arg_parser()

    args = parser.parse_args()
    project_root = find_project_root()
    os.chdir(project_root)

    if args.dist:
        args.flux_runtime = True
        args.flux_bootstrapper = True

    if args.flux_demo:
        args.flux_demo_cpp = True
        args.flux_demo_bootstrapped = True

    config = CmakeConfig(libraryType=args.library, isDist=args.dist, cppDemo=args.flux_demo_cpp, boostrappedDemo=args.flux_demo_bootstrapped)

    start_cmake(config)
    build_engine(args.configuration)

    if args.flux_runtime:
        build_runtime(args.configuration)

    if args.flux_bootstrapper:
        build_bootstrapper(args.configuration)

    if args.flux_demo_cpp:
        build_demo_cpp(args.configuration)

    if args.flux_demo_bootstrapped:
        build_demo_bootstrapped(args.configuration)

    if args.dist:
        package_dist(args.library)