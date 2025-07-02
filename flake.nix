{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    pre-commits.url = "github:cachix/git-hooks.nix";
    utils.url = "github:numtide/flake-utils";
  };
  outputs =
    {
      self,
      nixpkgs,
      utils,
      pre-commits,
    }@inputs:
    utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = import nixpkgs {
          inherit system;
        };
      in
      with pkgs;
      {
        checks = inputs.pre-commits.lib.${system}.run {
          src = ./.;
          hooks = {
            # C++ code style and formatting checks
            clang-format = {
              enable = true;
              types_or = lib.mkForce [
                "c"
                "c++"
              ];
            };
            gtest = {
              enable = false;
              name = "gtest-check";
              types = [ "c++" ];
              entry = "cmake --build build --target test";
              stages = [ "pre-commit" ];
            };
            # Spelling and grammar checks
            typos = {
              enable = false;
              settings.configuration = '''';
              settings.exclude = "*.nix";
              settings.ignored-words = [
                "NixOS"
                "Nixpkgs"
                "Knot-DI"
                "restat"
              ];
            };

            # Commit checker
            commitizen.enable = true;
          };
        };

        devShells.default = mkShell {
          name = "Knot-DI Development Shell";
          inherit (self.checks.${system}) shellHook;
          buildInputs = self.checks.${system}.enabledPackages;

          packages =
            with pkgs;
            [
              # Core packages
              conan
              cmake
              gtest
              llvm

              # Development tools
              cppcheck
              clang-tools
              ninja
              spdlog
              # Misc
              doxygen
              lcov
            ]
            ++ (
              if system == "x86_64-linux" then
                [
                  # Additional packages for x86_64-linux
                  libpcap
                  libpcap-devel
                  gdb
                ]
              else
                [ ]
            );
        };
      }
    );
}
