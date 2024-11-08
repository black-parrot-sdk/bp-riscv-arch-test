
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
	$(MKDIR) -p $(WORKDIR)/bin
	$(SED) "s#\$${TOP}#$(CURDIR)#g" config.ini.template > $@
	$(PYTHON) -m pip install $(RISCOF_URL)
	cd riscv-arch-test/riscv-ctg; $(PYTHON) -m pip install --editable .
	cd riscv-arch-test/riscv-isac; $(PYTHON) -m pip install --editable .

Z3_VERSION := z3-4.13.3
Z3_URL := https://github.com/Z3Prover/z3.git
$(WORKDIR)/bin/z3: | config.ini
	$(CD) $(WORKDIR); \
		$(GIT) clone -b $(Z3_VERSION) $(Z3_URL)
	$(CD) $(WORKDIR)/z3; \
		$(PYTHON) scripts/mk_make.py
	$(CD) $(WORKDIR)/z3/build; \
		$(MAKE) PREFIX=$(WORKDIR) all install

OPAMROOT := $(WORKDIR)/opam
OPAM_VERSION := 2.2.1
OPAM_BINARY := opam-$(OPAM_VERSION)-x86_64-linux
OPAM_URL := https://github.com/ocaml/opam/releases/download/$(OPAM_VERSION)/$(OPAM_BINARY)
$(WORKDIR)/bin/opam: | $(WORKDIR)/bin/z3
	$(WGET) $(OPAM_URL) -P $(WORKDIR)
	$(MV) $(WORKDIR)/$(OPAM_BINARY) $@
	$(MKEXE) $@
	$@ init -y --disable-sandboxing --root=$(OPAMROOT)
	$@ install sail -y --root=$(OPAMROOT)

$(WORKDIR)/bin/riscv_sim_RV64: $(WORKDIR)/bin/opam
	$(MAKE) ARCH=RV64 -C sail-riscv
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

