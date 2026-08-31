# FT-PER-05 — Source normative consolidée

## PER05-R01 — État acquisition après reboot

**Classification : `NOT_DEFINED`.**

Bloc 1 définit `acquisition_state`, mais aucune règle V1 n'impose sa valeur après reboot ni une reprise automatique.

## PER05-R02 — Campagne active après reboot

**Classification : `NOT_DEFINED`.**

Aucune règle n'impose le sort de `active_campaign_id` ou d'une campagne en cours lorsqu'un reboot intervient.

## PER05-R03 — Inventaire des campagnes après reboot

**Classification : `NOT_DEFINED`.**

Bloc 6 liste les campagnes enregistrées et expose le stockage, mais ne définit pas explicitement la politique de persistance/reconstruction de l'inventaire après reboot.

## PER05-R04 — Campagne interrompue par reboot

**Classification : `NOT_DEFINED`.**

La V1 ne fixe pas si elle doit devenir terminée, erreur, partiellement corrompue, rester en cours ou être supprimée.

## PER05-R05 — selected_campaign_index après reboot

**Classification : `NOT_DEFINED`.**

Le registre est RW pour naviguer dans l'inventaire, sans politique d'initialisation ou de conservation post-reboot.

## PER05-R06 — Compteurs de stockage après reboot

**Classification : `NOT_DEFINED` pour la propriété de persistance.**

`total_campaign_count`, `valid_campaign_count`, `storage_used_mb` et `storage_free_mb` ont une sémantique nominale, mais aucune règle de reprise post-reboot n'est spécifiée.

## PER05-R07 — Défauts/warnings tant que la condition est présente

**Classification : `DELEGATED` pour la sémantique nominale ; `NOT_DEFINED` pour la mémoire non volatile.**

Bloc 1 impose que défauts et avertissements persistent tant que la condition est présente. Cette règle ne définit pas une mémoire historique non volatile à travers reboot.

## PER05-R08 — last_fault_code / last_fault_timestamp après reboot

**Classification : `NOT_DEFINED`.**

Bloc 7 définit le dernier défaut significatif et son timestamp, sans règle de conservation post-reboot.

## PER05-R09 — selftest_* après reboot

**Classification : `NOT_DEFINED`.**

Les valeurs de statut/résultat d'autotest sont définies, mais leur rétention après reboot ne l'est pas.

## PER05-R10 — Statistiques et compteurs après reboot

**Classification : `NOT_DEFINED`.**

Aucune politique générale V1 ne classe les statistiques/compteurs en volatils, persistants, reconstruits au boot ou réinitialisés.

## PER05-R11 — RESET STATISTICS

**Classification : `DELEGATED` vers FT-CMD-07.**

RESET STATISTICS est une commande protégée distincte du reboot. Ses exclusions normatives ne peuvent pas être transformées en règle de persistance au reboot.

## PER05-R12 — reset_cause / uptime_s

**Classification : `DELEGATED` vers FT-PER-01.**

Ces observables de reboot ont déjà un propriétaire.

## PER05-R13 — Caractérisation globale avant/après RESET SOFTWARE

**Classification : `TRACE_ONLY`.**

Les états B1/B6/B7 peuvent être relevés avant/après pour préparer l'implémentation et une future V1.1, sans produire de verdict de persistance.

## Règles anti-fabrication

- « enregistré dans le capteur » ne suffit pas à définir une politique exhaustive de reprise après reboot ;
- « persistent tant que la condition est présente » ne signifie pas automatiquement « mémorisé en mémoire non volatile » ;
- ne pas assimiler RESET STATISTICS à un reset système ;
- ne pas imposer acquisition arrêtée ou reprise automatique au boot ;
- ne pas inventer le sort d'une campagne interrompue ;
- ne pas imposer de valeurs initiales B6/B7 non spécifiées.
