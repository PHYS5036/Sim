#!/usr/bin/env bash

if [[ "${BASH_SOURCE[0]}" == "$0" ]]; then
    echo "Please source this script: source setupLocal.sh" >&2
    exit 1
fi

if [[ -n "${ROOT_SETUP:-}" ]]; then
    if [[ ! -f "$ROOT_SETUP" ]]; then
        echo "ROOT_SETUP does not point to a file: $ROOT_SETUP" >&2
        return 1
    fi
    source "$ROOT_SETUP" || return 1
fi

if [[ -n "${GEANT4_SETUP:-}" ]]; then
    if [[ ! -f "$GEANT4_SETUP" ]]; then
        echo "GEANT4_SETUP does not point to a file: $GEANT4_SETUP" >&2
        return 1
    fi
    source "$GEANT4_SETUP" || return 1
fi

SIM_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="${PROJECT_DIR:-$SIM_DIR}"
OUTPUT_DIR="${OUTPUT_DIR:-$PROJECT_DIR/outputs}"
CONFIG_PATH="${CONFIG_PATH:-$PROJECT_DIR/config.json}"

export SIM_DIR PROJECT_DIR OUTPUT_DIR CONFIG_PATH
export PATH="$SIM_DIR/bin:$SIM_DIR/build/GammaLabMC:$SIM_DIR/build/DoseProfileMC:$SIM_DIR/GammaLabMC/build:$SIM_DIR/DoseProfileMC/build:$PATH"
