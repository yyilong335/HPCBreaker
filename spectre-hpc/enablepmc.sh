# install msr-tools for rdmsr and wrmsr:
# read/write Model Specific Registers
if ! dpkg -s msr-tools >/dev/null 2>&1; then
  sudo apt install msr-tools
fi

# chown and chmod + setuid so that you can run pmc.cpp code outside 
# root
chown root $(which wrmsr)
chown root $(which rdmsr)
chmod +s $(which wrmsr)
chmod +s $(which rdmsr)

# make sure kernel module is enabled
modprobe msr

# enable rdpmc instruction
echo 2 > /sys/bus/event_source/devices/cpu/rdpmc

# disable smt
#echo off > /sys/devices/system/cpu/smt/control

# disable hyper-threading
for i in $(seq 1 255)
do
  if [ -d "/sys/devices/system/cpu/cpu${i}" ]; then
    echo 0 > "$cpu/online"
  fi
done

echo 1 > /sys/devices/system/cpu/intel_pstate/no_turbo
