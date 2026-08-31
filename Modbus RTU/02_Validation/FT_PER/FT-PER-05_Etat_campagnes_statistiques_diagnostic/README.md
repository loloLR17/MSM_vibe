# FT-PER-05 — État système, campagnes, statistiques et diagnostic

## 1. Objet

FT-PER-05 audite les propriétés de persistance post-reboot relatives à l'état système, l'acquisition, les campagnes stockées, les statistiques et le diagnostic.

La règle est stricte : une donnée exposée avant et après reboot n'est pas réputée persistante sans exigence normative explicite.

## 2. Conclusions V1

La V1 définit les états courants et les inventaires, mais ne définit pas leur politique générale de rétention après reboot.

En particulier, aucune règle normative n'impose après RESET SOFTWARE :

- un `acquisition_state` déterminé ;
- la reprise automatique d'une acquisition ;
- la conservation ou suppression d'une campagne interrompue ;
- la conservation de `selected_campaign_index` ;
- une politique explicite de persistance de l'inventaire B6 ;
- la conservation de `last_fault_code` / `last_fault_timestamp` ;
- la conservation du résultat d'autotest ;
- une politique générale de persistance des statistiques/compteurs.

## 3. Nuance importante — défauts et avertissements

Bloc 1 impose que les défauts et avertissements soient persistants **tant que la condition est présente**.

Cette phrase est interprétée comme une exigence de sémantique de condition active, et non comme une exigence explicite de stockage non volatil d'un historique à travers reboot.

FT-PER-05 ne transforme donc pas cette règle en garantie de mémorisation post-reboot.

## 4. Campagnes

Bloc 6 expose des campagnes « enregistrées dans le capteur » et un inventaire de stockage. Cela décrit leur fonction et leur présence dans le stockage, mais la V1 ne fournit pas de règle explicite de reprise après reboot/coupure ni de traitement d'une campagne interrompue.

La conservation observée peut être caractérisée mais ne devient pas un oracle FT-PER V1.

## 5. RESET STATISTICS

La commande 11 est déléguée à FT-CMD-07. Elle ne doit pas être assimilée à un reboot.

La V1 indique notamment que RESET STATISTICS ne doit pas effacer les campagnes, l'identité capteur, la configuration et les logs critiques, mais son périmètre positif exact reste incomplet. FT-PER-05 ne déduit pas de cette commande une politique générale de persistance au reboot.

## 6. Diagnostic

`reset_cause` et `uptime_s` sont déjà traités par FT-PER-01.

Les autres historiques/états B7 (`last_fault_*`, `selftest_*`) n'ont pas de politique post-reboot normative explicite.

## 7. Test de caractérisation

- `TT-PER-B01B06B07-001` — relevé état/acquisition/campagnes/diagnostic avant et après RESET SOFTWARE (`TRACE_ONLY`).

## 8. Frontières

- cohérences nominales acquisition/campagnes : FT-INT-04 ;
- diagnostic transversal nominal : FT-INT-05 ;
- RESET STATISTICS : FT-CMD-07 ;
- cause reset / uptime : FT-PER-01 ;
- power cycle et reprise générale : FT-PER-06.

## 9. Statut

Sous-famille reconstruite pour revue. Aucun gel ni merge sans validation explicite.
