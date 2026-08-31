# FT-PER-03 — Cas détaillé

## TT-PER-B04-001 — Caractérisation du Bloc 4 avant / après RESET SOFTWARE

**Classification : `TRACE_ONLY`.**

### Objectif

Observer sans transformer en exigence V1 le comportement des données et états de configuration à travers un redémarrage logiciel contrôlé.

### Sources

- Bloc 4 V1 ;
- FT-CMD-05 ;
- FT-INT-02 ;
- FT-PER-01.

### Préconditions

- configuration préparée connue ;
- si possible, configuration appliquée avec succès avant reboot afin de disposer d'une image active exploitable ;
- RESET SOFTWARE exécutable dans les conditions normatives ;
- reboot établissable selon FT-PER-01.

### Capture avant reboot

Relever au minimum :

- `prepared_config_id` ;
- `active_config_id` ;
- `config_state` ;
- `config_error_code` ;
- `prepared_config_crc` ;
- `active_config_crc` ;
- `config_revision_counter` ;
- un échantillon représentatif, ou l'intégralité si le moyen le permet, des zones préparées 4B/4C/4D ;
- l'image active 4E.

### Étapes

1. établir un état Bloc 4 connu et cohérent ;
2. effectuer la capture pré-reboot ;
3. provoquer RESET SOFTWARE conformément à FT-PER-01 / FT-CMD-07 ;
4. confirmer que le reboot a réellement eu lieu ;
5. reprendre les lectures Modbus sans imposer de délai V1 ;
6. effectuer la même capture post-reboot ;
7. comparer les valeurs et classer les observations par propriété.

### Résultat V1

Aucun PASS/FAIL n'est produit sur la conservation ou la remise à zéro des données de configuration, car la politique post-reboot n'est pas définie par V1.

Le résultat du scénario est une trace de caractérisation, par exemple :

- conservé ;
- remis à zéro ;
- reconstruit ;
- modifié ;
- non observable.

Ces termes décrivent l'implémentation testée et ne deviennent pas des exigences V1.

### FAIL possibles

Aucun FAIL FT-PER-03 n'est défini sur la valeur post-reboot elle-même.

Une anomalie relevant d'une autre famille peut toutefois être signalée séparément, par exemple :

- valeur réservée de `config_state` produite par l'implémentation V1 ;
- incohérence structurelle d'un `uint32` ;
- impossibilité de lire une zone qui devrait normalement être accessible.

Ces anomalies doivent être attribuées à leur famille propriétaire, pas à la persistance FT-PER-03.

### Non-conclusions obligatoires

Il est interdit de conclure à partir d'un seul essai que :

- la configuration active est normativement persistante ;
- la zone préparée est normativement volatile ;
- le compteur de révision est normativement non volatil ;
- le firmware doit restaurer `ACTIF` après boot.
