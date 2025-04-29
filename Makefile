
TOP                ?= $(shell git rev-parse --show-toplevel)
BP_SDK_DIR         ?= $(TOP)/..
BP_SDK_INSTALL_DIR ?= $(BP_SDK_DIR)/install
BP_SDK_BIN_DIR     ?= $(BP_SDK_INSTALL_DIR)/bin

WORKDIR            := $(TOP)/work

CP := cp
MKDIR := mkdir -p
GIT := git
CD := cd
WGET := wget
MV := mv
PYTHON := python3
PIP := pip3
RISCOF := riscof
CAT := cat
SED := sed
ENVSUBST := envsubst
MKEXE := chmod +x
CURL := curl
TAR := tar


RISCOF_URL ?= git+https://github.com/riscv/riscof.git
config.ini:
	$(GIT) submodule update --init --recursive
	$(SED) "s#\$${TOP}#$(CURDIR)#g" config.ini.template > $@
	$(PYTHON) -m pip install $(RISCOF_URL)
	$(CD) riscv-arch-test/riscv-ctg; $(PYTHON) -m pip install --editable .
	$(CD) riscv-arch-test/riscv-isac; $(PYTHON) -m pip install --editable .

$(WORKDIR)/bin/riscv_sim_RV64: config.ini
	$(MKDIR) -p $(@D)
	$(MAKE) ARCH=RV64 -C sail-riscv
	$(PIP) install riscof
	$(CP) sail-riscv/c_emulator/riscv_sim_RV64 $@

RVARCH_DIR := $(TOP)/riscv-arch-test
RVTEST_DIR := $(RVARCH_DIR)/riscv-test-suite
$(WORKDIR)/riscof/A/src/amoadd.d-01.S/dut/amoadd.d-01.riscv: $(WORKDIR)/bin/riscv_sim_RV64
	$(RISCOF) validateyaml --work-dir=$(WORKDIR)/riscof --config=config.ini
	$(RISCOF) run --work-dir=$(WORKDIR)/riscof --config=config.ini --suite=$(RVTEST_DIR)/rv64i_m --env=$(RVTEST_DIR)/env --no-browser || true

all: $(WORKDIR)/riscof/A/src/amoadd.d-01.S/dut/amoadd.d-01.riscv

clean:
	rm -rf config.ini
	rm -rf work

PATH := $(BP_SDK_BIN_DIR):$(PATH)
PATH := $(WORKDIR)/bin:$(PATH)
PATH := $(OPAMROOT)/default/bin:$(PATH)

