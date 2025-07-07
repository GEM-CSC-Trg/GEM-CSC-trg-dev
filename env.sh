run_cmd() {
  # echo "> $@"
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
  export CMSSW_VER=$1
  export SCRAM_ARCH=$2
  echo "when installing cmssw"
  echo "$@"
  if ! [ -f "$ANALYSIS_SOFT_PATH/$CMSSW_VER/.installed" ]; then
    run_cmd mkdir -p "$ANALYSIS_SOFT_PATH"
    cd "$ANALYSIS_SOFT_PATH"
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
    run_cmd touch "$ANALYSIS_SOFT_PATH/$CMSSW_VER/.installed"
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

load_flaf_env() {
  local cmssw_version=${1:-CMSSW_14_1_7} # Default CMSSW version if not provided
  [ -z "$FLAF_ENVIRONMENT_PATH" ] && export FLAF_ENVIRONMENT_PATH="/afs/cern.ch/work/k/kandroso/public/flaf_env_2025_04"

  [ -z "$LAW_HOME" ] && export LAW_HOME="$ANALYSIS_PATH/.law"
  [ -z "$LAW_CONFIG_FILE" ] && export LAW_CONFIG_FILE="$ANALYSIS_PATH/config/law.cfg"
  [ -z "$ANALYSIS_DATA_PATH" ] && export ANALYSIS_DATA_PATH="$ANALYSIS_PATH/data"
  [ -z "$ANALYSIS_BIN_PATH" ] && export ANALYSIS_BIN_PATH="$ANALYSIS_PATH/bin"
  [ -z "$X509_USER_PROXY" ] && export X509_USER_PROXY="$ANALYSIS_DATA_PATH/voms.proxy"

  if [[ ! -d "$ANALYSIS_DATA_PATH" ]]; then
    run_cmd mkdir -p "$ANALYSIS_DATA_PATH"
  fi
  if [[ ! -d "$ANALYSIS_BIN_PATH" ]]; then
    run_cmd mkdir -p "$ANALYSIS_BIN_PATH"
  fi

  local os_version=$(cat /etc/os-release | grep VERSION_ID | sed -E 's/VERSION_ID="([0-9]+).*"/\1/')
  local os_prefix=$(get_os_prefix $os_version)
  local node_os=$os_prefix$os_version

  local target_os_version=9
  local target_os_prefix=$(get_os_prefix $target_os_version)
  local target_os_gt_prefix=$(get_os_prefix $target_os_version 1)
  local target_os=$target_os_prefix$target_os_version
  export FLAF_CMSSW_BASE="$ANALYSIS_PATH/soft/$cmssw_version"
  export FLAF_CMSSW_ARCH="${target_os_gt_prefix}${target_os_version}_amd64_gcc12"
  echo "Setting up environment for $cmssw_version"

  run_cmd install_cmssw "$env_file" $node_os $target_os $FLAF_CMSSW_ARCH $cmssw_version

  export PYTHONPATH="$ANALYSIS_PATH:$PYTHONPATH"

  if [ ! -z $ZSH_VERSION ]; then
    autoload bashcompinit
    bashcompinit
  fi
  source "$FLAF_ENVIRONMENT_PATH/bin/activate"
  source "$( law completion )"
  current_args=( "$@" )
  set --
  source /cvmfs/cms.cern.ch/rucio/setup-py3.sh &> /dev/null
  set -- "${current_args[@]}"
  export PATH="$ANALYSIS_SOFT_PATH/bin:$PATH"
  alias cmsEnv="env -i HOME=$HOME ANALYSIS_PATH=$ANALYSIS_PATH ANALYSIS_DATA_PATH=$ANALYSIS_DATA_PATH X509_USER_PROXY=$X509_USER_PROXY FLAF_CMSSW_BASE=$FLAF_CMSSW_BASE FLAF_CMSSW_ARCH=$FLAF_CMSSW_ARCH $FLAF_PATH/cmsEnv.sh"
  # echo ${cmsEnv}
}

source_env_fn() {
  local env_file="$1"
  local cmssw_version="$2"
  local cmd="$3"
  # echo "env_file = $env_file"
  # echo "cmd = $cmd"
  # echo "cmssw_version = $cmssw_version"
  # echo "$@"

  local this_file="$( [ ! -z "$ZSH_VERSION" ] && echo "${(%):-%x}" || echo "${BASH_SOURCE[0]}" )"
  local this_dir="$( cd "$( dirname "$this_file" )" && pwd )"

  export FLAF_PATH="$this_dir"

  if [ -z "$ANALYSIS_PATH" ]; then
    echo "ANALYSIS_PATH is not set. Exiting..."
    kill -INT $$
  fi

  [ -z "$ANALYSIS_SOFT_PATH" ] && export ANALYSIS_SOFT_PATH="$ANALYSIS_PATH/soft"

  if [ "$cmd" = "install_cmssw" ]; then
    do_install_cmssw "$cmssw_version" "${@:4}"
  else
    load_flaf_env $cmssw_version
  fi
}

source_env_fn "$@"

unset -f run_cmd
unset -f get_os_prefix
unset -f do_install_cmssw
unset -f install
unset -f install_cmssw
unset -f load_flaf_env
unset -f source_env_fn