# FT-STR-01 — Tests instanciés

## Objet
Tests instanciés de conformité structurelle globale à partir du mapping unifié brut et de la couverture par bloc.

## Principe
- 1 fiche de test de structure par bloc
- 1 fiche de test globale inter-blocs
- vérification des plages couvertes
- vérification de l'absence de chevauchement
- vérification de l'absence de trous à l'intérieur de la portée couverte du bloc

## Convention d’identifiant
- `TT-STR-01-B<bloc>-001` pour les tests bloc
- `TT-STR-01-GLOBAL-001` pour le test global

- Bloc 0 : portée 0..20, 21 registre(s) couverts, 0 trou(x) interne(s)
- Bloc 1 : portée 1000..1019, 20 registre(s) couverts, 0 trou(x) interne(s)
- Bloc 2 : portée 2000..2015, 16 registre(s) couverts, 0 trou(x) interne(s)
- Bloc 3 : portée 3000..3047, 48 registre(s) couverts, 0 trou(x) interne(s)
- Bloc 4 : portée 4000..4175, 176 registre(s) couverts, 0 trou(x) interne(s)
- Bloc 5 : portée 5000..5019, 20 registre(s) couverts, 0 trou(x) interne(s)
- Bloc 6 : portée 6000..6063, 64 registre(s) couverts, 0 trou(x) interne(s)
- Bloc 7 : portée 7000..7015, 16 registre(s) couverts, 0 trou(x) interne(s)