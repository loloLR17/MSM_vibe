# Source this file from Bash. No global SDK or security configuration.
export MSM_DEV_ROOT="${MSM_DEV_ROOT:-$HOME/dev/msm}"
export TR2_REPO="$MSM_DEV_ROOT/projects/MSM_vibe"
export STM32CUBE_U5_ROOT="$MSM_DEV_ROOT/tools/STM32CubeU5-v1.9.0"
export PATH="$HOME/.local/bin:$MSM_DEV_ROOT/scripts:$PATH"
