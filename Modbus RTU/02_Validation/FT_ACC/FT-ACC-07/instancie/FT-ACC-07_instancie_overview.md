# FT-ACC-07 — Tests instanciés

## Objet
Tests instanciés à partir du mapping unifié logique TR2 pour valider la conformité globale mapping ↔ comportement.

## Règles appliquées
- RO : lecture OK, écriture refusée avec exception explicite
- RW : lecture OK, écriture OK, write → read cohérent
- reserved* : lecture exploitable si exposée, écriture refusée ou sans effet observable

## Couverture par bloc
- Bloc 0 : 10 champ(s)
- Bloc 1 : 18 champ(s)
- Bloc 2 : 12 champ(s)
- Bloc 3 : 26 champ(s)
- Bloc 4 : 66 champ(s)
- Bloc 5 : 18 champ(s)
- Bloc 6 : 20 champ(s)
- Bloc 7 : 13 champ(s)

## Répartition par type d’accès
- RO : 130
- RW : 35
- reserved* : 18

## Total
- 183 champs instanciés