#!/usr/bin/env bash
set -euo pipefail

# Clone the heavyweight dependencies next to the main Codes repo.
# This keeps the main repo source-only while still making the full working tree easy to reconstruct.

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

clone_repo() {
  local url="$1"
  local dest="$2"

  if [[ -d "${dest}/.git" ]]; then
    echo "Skipping ${dest} (already cloned)"
    return 0
  fi

  if [[ -e "${dest}" ]]; then
    echo "Skipping ${dest} (path exists but is not a git clone)"
    return 0
  fi

  echo "Cloning ${url} -> ${dest}"
  git clone "${url}" "${dest}"
}

clone_repo "https://github.com/AMReX-Codes/amrex.git" "${ROOT_DIR}/amrex"
clone_repo "https://github.com/AMReX-Codes/amrex-tutorials.git" "${ROOT_DIR}/amrex-tutorials"
clone_repo "https://github.com/AMReX-Codes/Amrvis.git" "${ROOT_DIR}/Amrvis"
clone_repo "https://github.com/AMReX-FHD/FHDeX.git" "${ROOT_DIR}/FHDeX"
clone_repo "https://ccse.lbl.gov/pub/Downloads/volpack.git" "${ROOT_DIR}/volpack"
