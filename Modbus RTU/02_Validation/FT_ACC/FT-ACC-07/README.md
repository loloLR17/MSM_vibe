# FT-ACC-07 — README

## Objet
Valider que le comportement réel du système est strictement conforme au mapping unifié.

## Règles retenues
- champ `RO` : lecture autorisée, écriture refusée avec exception explicite ;
- champ `RW` : lecture autorisée, écriture autorisée, write → read cohérent ;
- champ `reserved*` : comportement neutre, lecture exploitable si exposée, écriture refusée ou sans effet observable.

## Structure
```text
FT-ACC-07/
├── source/
├── detaille/
├── instancie/
└── archive_pre_renforcement/
```

## Logique d’instanciation
- une fiche par champ du mapping logique ;
- verdict attendu dérivé automatiquement du type d’accès et de la règle `reserved*`.
