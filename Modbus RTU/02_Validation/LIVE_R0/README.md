# Validation du profil LIVE — R0

Statut : **SPECIFIED_NOT_EXECUTED** pour tous les tests de fonctionnalité.
Norme : [profil LIVE](../../01_Specification_source/LIVE_R0/PROFIL_LIVE.md) et [migration](../../01_Specification_source/LIVE_R0/MIGRATION_FRAM.md).

- `mapping_live.csv` : mapping complet des 8 blocs, 187 champs / 390 registres. Reprend les sept blocs V1 et B5 V1.1 ; sources, adresses, types et accès conservés. Les spécialités LIVE sont identifiées par exigence. Il ne remplace aucun CSV gelé.
- `tests_live.csv` : 106 scénarios successeurs, avec exigence, famille, oracle, niveau et référence historique lorsqu'elle est directement pertinente. Un scénario paramétré peut nécessiter plusieurs exécutions futures. Le nombre106 n'est ni un nombre de tests implémentés ni un nombre de PASS.
- Les contrôles documentaires de mapping, réservations, CRC et géométrie sont consignés dans les preuves R0, distinctement des tests firmware/PC ou physiques.

FT-LIM couvre tuples/domaines/durée/CRC et frontières ; FT-CMD couvre commandes, priorité et identité ; FT-INT et BLK couvrent états, calculs, qualité et alarmes ; FT-SEQ sépare cycles LIVE/campagne ; FT-PER couvre coupures, compatibilité et rollback ; FT-OBS couvre absence volontaire, panne, validité et inventaire. Les contrôles structurels et d'accès FT-STR/ACC s'appliquent toujours.

Les TT historiques demeurent leurs propres oracles V1/V1.1. En particulier TT-SEQ-CAMP-001/002 doivent encore prouver une campagne réelle ; ils ne sont pas rendus PASS par un démarrage LIVE. Le corpus A06 est réutilisé et son intégrité contrôlée ; aucune régénération/suppression des anciens instanciés.

Le statut d'un test logiciel ne passe à PASS qu'après exécution de la future implémentation avec sortie conservée. R1/R2 doivent notamment tester les vrais stores et l'epoch, pas seulement le modèle documentaire. R6 doit produire les preuves physiques avec SHA image, identité et autorisations de sa mission. R0 ne donne aucune permission matérielle supplémentaire.

`decisions_start_stop.csv` explicite 384 contextes START/STOP LIVE après validation de la mailbox et de l’identité transactionnelle. Ce sont des oracles documentaires, sans exécution du moteur de commandes. Une admission n’est pas une preuve de succès.
