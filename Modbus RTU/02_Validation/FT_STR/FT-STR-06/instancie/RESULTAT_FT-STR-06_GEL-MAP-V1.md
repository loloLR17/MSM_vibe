# Résultat FT-STR-06 — GEL-MAP-V1

## Portée

Cette fiche distingue la conformité démontrable mécaniquement depuis GEL-MAP-V1 de la conformité nécessitant une exécution Modbus réelle.

## Constat structurel

GEL-MAP-V1 expose 8 plages continues :
- B0 : 0..20 (21 registres) ;
- B1 : 1000..1019 (20) ;
- B2 : 2000..2015 (16) ;
- B3 : 3000..3047 (48) ;
- B4 : 4000..4175 (176) ;
- B5 : 5000..5019 (20) ;
- B6 : 6000..6063 (64) ;
- B7 : 7000..7015 (16).

Sept lacunes séparent ces plages. Elles ne sont pas exposées et une requête les traversant est invalide.

Le Bloc 4 dépasse la limite FC03 de 125 registres et doit donc être lu par segmentation. Cette situation est normale et ne constitue pas une anomalie.

## Ce que le validateur mécanique peut établir

- continuité de chaque bloc exposé ;
- bornes et longueurs des 8 blocs ;
- présence des lacunes inter-blocs ;
- identification des blocs nécessitant une segmentation FC03 ;
- cohérence de la couverture instanciée avec GEL-MAP-V1.

## Ce qui nécessite une exécution cible

- acceptation effective des lectures unitaires et sous-plages valides ;
- acceptation des lectures traversant plusieurs champs logiques contigus ;
- acceptation de la segmentation du Bloc 4 ;
- exception Modbus appropriée sur adresses/lacunes invalides ;
- rejet des quantités 0 et >125 ;
- absence de réponse partielle et d'effet de bord sur requête invalide.

## Statut

**Structure de validation reconstruite et alignée sur V1 / GEL-MAP-V1 / GEL-GOV-02.**

La conformité d'implémentation Modbus reste **À EXÉCUTER SUR CIBLE**. Aucun résultat terrain n'est fabriqué à partir du mapping.
