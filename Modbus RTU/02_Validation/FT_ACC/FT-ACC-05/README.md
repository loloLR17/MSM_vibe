# FT-ACC-05 — README

## Objet
Valider l'absence d'effets de bord des écritures Modbus autorisées.

## Stratégie de snapshot
**Option B — Snapshot bloc complet**
- snapshot complet du bloc cible avant écriture ;
- snapshot complet du bloc cible après écriture ;
- diff limité strictement à la cible adressée.

## Structure
- `source/` : spécification
- `detaille/` : cas génériques
- `instancie/` : cas issus du mapping réel
