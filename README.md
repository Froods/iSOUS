# iSOUS

## Python setup

Projektet bruger Python dependencies fra `requirements.txt`.

Du kan enten bruge `venv` eller `conda`.

## Mulighed 1: Setup med venv

Kør kommandoerne fra projektets rodmappe.

### macOS/Linux

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
```

### Windows PowerShell

```powershell
python -m venv .venv
.\.venv\Scripts\Activate.ps1
pip install -r requirements.txt
```

## Mulighed 2: Setup med conda

Kør kommandoerne fra projektets rodmappe.

```bash
conda create -n isous python=3.11
conda activate isous
pip install -r requirements.txt
```

## Kør GUI

GUI'en startes med:

```bash
python GUI/main.py
```
