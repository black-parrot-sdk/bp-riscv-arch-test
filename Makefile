
TOP                ?= $(shell git rev-parse --show-toplevel)
BP_SDK_DIR         ?= $(TOP)/..
BP_SDK_INSTALL_DIR ?= $(BP_SDK_DIR)/install
BP_SDK_BIN_DIR     ?= $(BP_SDK_INSTALL_DIR)/bin

WORKDIR            := $(TOP)/work

OPAM := opam
CP := cp
MKDIR := mkdir -p
GIT := git
CD := cd
WGET := wget
MV := mv
PYTHON := python3
RISCOF := riscof
CAT := cat
ENVSUBST := envsubst

checkout:
	git submodule update --init --recursive

OPAMROOT := $(WORKDIR)/opam
OPAM_VERSION := 2.1.5
OPAM_BINARY := opam-$(OPAM_VERSION)-x86_64-linux
OPAM_URL := https://github.com/ocaml/opam/releases/download/$(OPAM_VERSION)/$(OPAM_BINARY)
opam: $(WORKDIR)/bin/opam
$(WORKDIR)/bin/opam:
	$(MKDIR) $(WORKDIR)/bin
	$(CD) $(WORKDIR); \
		$(WGET) $(OPAM_URL); \
		chmod +x $(OPAM_BINARY); \
		$(MV) $(OPAM_BINARY) $(WORKDIR)/bin/opam
	OPAMROOT=$(OPAMROOT) $(OPAM) init -y --disable-sandboxing
	OPAMROOT=$(OPAMROOT) $(OPAM) install sail -y

Z3_VERSION := z3-4.11.2
Z3_URL := https://github.com/Z3Prover/z3.git
z3: $(WORKDIR)/bin/z3
$(WORKDIR)/bin/z3:
	$(MKDIR) $(WORKDIR)/bin
	$(CD) $(WORKDIR); \
		$(GIT) clone -b $(Z3_VERSION) $(Z3_URL)
	$(CD) $(WORKDIR)/z3; \
		$(PYTHON) scripts/mk_make.py
	$(CD) $(WORKDIR)/z3/build; \
		$(MAKE) PREFIX=$(WORKDIR) all install

SAIL_DIR := $(WORKDIR)/sail-riscv
SAIL_URL := https://github.com/riscv/sail-riscv.git
SAIL_VERSION := d7a3d8012fd579f40e53a29569141d72dd5e0c32
sail: $(WORKDIR)/bin/riscv_sim_RV64
$(WORKDIR)/bin/riscv_sim_RV64: $(WORKDIR)/bin/opam
	$(MKDIR) $(WORKDIR)/bin
	$(GIT) clone $(SAIL_URL) $(SAIL_DIR)
	cd $(SAIL_DIR); $(GIT) checkout $(SAIL_VERSION)
	$(MAKE) -C $(SAIL_DIR) all
	$(CP) $(SAIL_DIR)/c_emulator/riscv_sim_RV64 $(WORKDIR)/bin
	$(PYTHON) -m pip install riscof

export TOP
config.ini:
	$(CAT) config.ini.template | $(ENVSUBST) > $@

RVARCH_DIR := $(TOP)/riscv-arch-test
RVTEST_DIR := $(RVARCH_DIR)/riscv-test-suite
all: opam z3 sail config.ini
	$(GIT) submodule update --init --recursive
	rm -rf $(WORKDIR)/riscof
	$(RISCOF) validateyaml --work-dir=$(WORKDIR)/riscof --config=config.ini
	$(RISCOF) run --work-dir=$(WORKDIR)/riscof --config=config.ini --suite=$(RVTEST_DIR)/rv64i_m --env=$(RVTEST_DIR)/env --no-browser || true

clean:
	rm -rf config.ini
	rm -rf work/riscof_work

PATH := $(BP_SDK_BIN_DIR):$(PATH)
PATH := $(WORKDIR)/bin:$(PATH)
PATH := $(OPAMROOT)/default/bin:$(PATH)

