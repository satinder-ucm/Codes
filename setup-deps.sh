#!/usr/bin/env bash
set -euo pipefail

# Placeholder for machine-local setup.
# Keep compiler, library, and environment setup outside Git so both machines can share the same source tree.

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if [[ -f "${ROOT_DIR}/setup-local.sh" ]]; then
  # shellcheck source=/dev/null
  source "${ROOT_DIR}/setup-local.sh"
else
  echo "No setup-local.sh found. Create one here for machine-specific environment setup if needed."
fi
