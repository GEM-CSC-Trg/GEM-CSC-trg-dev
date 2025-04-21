#!/usr/bin/env bash

run_cmd() {
  "$@"
  RESULT=$?
  if (( $RESULT != 0 )); then
    echo "Error while running '$@'"
    kill -INT $$
  fi
}

get_os_prefix() {
  local os_version=$1
  local for_global_tag=$2
  if (( $os_version >= 8 )); then
    echo el
  elif (( $os_version < 6 )); then
    echo error
  else
    if [[ $for_global_tag == 1 || $os_version == 6 ]]; then
      echo slc
    else
      echo cc
    fi
  fi
}

do_install_cmssw() {
  export SCRAM_ARCH=$1
  local CMSSW_VER=$2
    if ! [ -f "$this_dir/soft/$CMSSW_VER/.installed" ]; then
    run_cmd mkdir -p "$this_dir/soft"
    run_cmd cd "$this_dir/soft"
    run_cmd source /cvmfs/cms.cern.ch/cmsset_default.sh
    if [ -d $CMSSW_VER ]; then
      echo "Removing incomplete $CMSSW_VER installation..."
      run_cmd rm -rf $CMSSW_VER
    fi
    echo "Creating $CMSSW_VER area in $PWD ..."
    run_cmd scramv1 project CMSSW $CMSSW_VER
    run_cmd cd $CMSSW_VER/src
    run_cmd eval `scramv1 runtime -sh`

    if [[ $(type -t apply_cmssw_customization_steps) == function ]] ; then
      run_cmd apply_cmssw_customization_steps
    fi
    run_cmd scram b -j8
    run_cmd cd "$this_dir"
    run_cmd touch "$this_dir/soft/$CMSSW_VER/.installed"
  fi
}

install() {
  local env_file="$1"
  local node_os=$2
  local target_os=$3
  local cmd_to_run=$4
  local installed_flag=$5

  if [ -f "$installed_flag" ]; then
    return 0
  fi

  if [[ $node_os == $target_os ]]; then
    local env_cmd=""
    local env_cmd_args=""
  else
    local env_cmd="cmssw-$target_os"
    if ! command -v $env_cmd &> /dev/null; then
      echo "Unable to do a cross-platform installation. $env_cmd is not available."
      return 1
    fi
    local env_cmd_args="--command-to-run"
  fi

  run_cmd $env_cmd $env_cmd_args /usr/bin/env -i HOME=$HOME bash "$env_file" $cmd_to_run "${@:6}"
}

install_cmssw() {
  local env_file="$1"
  local node_os=$2
  local target_os=$3
  local scram_arch=$4
  local cmssw_version=$5
  install "$env_file" $node_os $target_os install_cmssw "$ANALYSIS_SOFT_PATH/$cmssw_version/.installed" "$scram_arch" "$cmssw_version"
}


load_env() {
  local env_file="$1"
  local this_file="$( [ ! -z "$ZSH_VERSION" ] && echo "${(%):-%x}" || echo "${BASH_SOURCE[0]}" )"
  local this_dir="$( cd "$( dirname "$this_file" )" && pwd )"

  export ANALYSIS_PATH="$this_dir"
  export PYTHONPATH="$ANALYSIS_PATH:$PYTHONPATH" # can be moved according to needs (e.g. in law there is hhInference)
  [ -z "$LAW_HOME" ] && export LAW_HOME="$ANALYSIS_PATH/.law"
  [ -z "$LAW_CONFIG_FILE" ] && export LAW_CONFIG_FILE="$ANALYSIS_PATH/config/law.cfg"
  [ -z "$LAW_HOME" ] && export LAW_HOME="$ANALYSIS_PATH/.law"

  [ -z "$ANALYSIS_DATA_PATH" ] && export ANALYSIS_DATA_PATH="$ANALYSIS_PATH/data"
  [ -z "$X509_USER_PROXY" ] && export X509_USER_PROXY="$ANALYSIS_DATA_PATH/voms.proxy"

  if [[ ! -d "$ANALYSIS_DATA_PATH" ]]; then
    run_cmd mkdir -p "$ANALYSIS_DATA_PATH"
  fi

  local os_version=$(cat /etc/os-release | grep VERSION_ID | sed -E 's/VERSION_ID="([0-9]+).*"/\1/')
  local os_prefix=$(get_os_prefix $os_version)
  local node_os=$os_prefix$os_version

  local cmssw_ver=CMSSW_14_2_0_pre1
  local target_os_version=9
  local target_os_prefix=$(get_os_prefix $target_os_version)
  local target_os_gt_prefix=$(get_os_prefix $target_os_version 1)
  local target_os=$target_os_prefix$target_os_version

  export CMSSW_BASE="$ANALYSIS_PATH/soft/$cmssw_ver"
  export CMSSW_ARCH="${target_os_gt_prefix}${target_os_version}_amd64_gcc12"

  install_cmssw "$env_file" $node_os $target_os $CMSSW_ARCH $cmssw_ver

  if [ ! -z $ZSH_VERSION ]; then
    autoload bashcompinit
    bashcompinit
  fi
  source /cvmfs/sft.cern.ch/lcg/views/setupViews.sh LCG_105 x86_64-${os_prefix}${os_version}-gcc13-opt
  source /afs/cern.ch/user/m/mrieger/public/law_sw/setup.sh

  source "$( law completion )"
  current_args=( "$@" )
  set --
  source /cvmfs/cms.cern.ch/rucio/setup-py3.sh &> /dev/null
  set -- "${current_args[@]}"

    #   if [[ $node_os == $target_os ]]; then
    #     export CMSSW_SINGULARITY=""
    #     local env_cmd=""
    #   else
    #     export CMSSW_SINGULARITY="/cvmfs/cms.cern.ch/common/cmssw-$target_os"
    #     local env_cmd="$CMSSW_SINGULARITY --command-to-run"
    #   fi
  export PATH="$ANALYSIS_SOFT_PATH/bin:$PATH"
  alias cmsEnv="env -i HOME=$HOME ANALYSIS_PATH=$ANALYSIS_PATH ANALYSIS_DATA_PATH=$ANALYSIS_DATA_PATH X509_USER_PROXY=$X509_USER_PROXY FLAF_CMSSW_BASE=$FLAF_CMSSW_BASE FLAF_CMSSW_ARCH=$FLAF_CMSSW_ARCH $ANALYSIS_PATH/cmsEnv.sh"
}


source_env_fn() {
  local env_file="$1"
  local cmd="$2"

  if [ -z "$ANALYSIS_PATH" ]; then
    echo "ANALYSIS_PATH is not set. Exiting..."
    kill -INT $$
  fi

  [ -z "$ANALYSIS_SOFT_PATH" ] && export ANALYSIS_SOFT_PATH="$ANALYSIS_PATH/soft"

  if [ "$cmd" = "install_cmssw" ]; then
    do_install_cmssw "${@:3}"
  else
    load_env "$env_file"
  fi
}


source_env_fn "$@"

unset -f run_cmd
unset -f get_os_prefix
unset -f do_install_cmssw
unset -f install
unset -f install_cmssw
unset -f load_env
unset -f source_env_fn