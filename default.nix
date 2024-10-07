with import <nixpkgs> {};
stdenv.mkDerivation {
  name = "zigc";
  buildInputs = [
    libbsd
  ];

  src = builtins.filterSource
    (path: type: baseNameOf path != ".git"
                 && baseNameOf path != "default.nix"
                 && baseNameOf path != "result")
    ./.;

  #buildPhase = ''
  #  make
  #'';

  installPhase = ''
    mkdir -p $out/bin/
    cp zigc $out/bin/
  '';
}
