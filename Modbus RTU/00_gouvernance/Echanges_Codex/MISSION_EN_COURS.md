---
mission_id: TR2-20261013-M016-SDMMC-1BIT-ENDURANCE
status: READY
base_ref: main
base_sha: 50c78fa076921b3126f59867f5caf5dca351ccd5
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M016 — Qualification débit, latence et endurance SDMMC2 1 bit sur banc indépendant

## Décision utilisateur impérative
Suspendre le diagnostic 4 bits. L'utilisateur impose la qualification **SDMMC2 1 bit**, avec le **code de démonstration indépendant dérivé de ST**, à des volumes, cadences et durées significatifs pour le TR2. L'utilisateur ne dispose pas d'oscilloscope et n'en aura pas à court terme : **ne pas en demander** ; utiliser les mesures numériques et les preuves logicielles/hardware disponibles. Aucun changement de gouvernance requis.

## Références de travail
Consulter HEAD réel de main, AGENTS.md, GOUVERNANCE_CHATGPT_CODEX.md, rapport GPIO DONE, rapports SDMMC physiques, README/provenance `experiments/sdmmc_st_reference`, arbitrage H3h-A, gel P8-B, validation H3h-C2a/b/c et contrats de stockage. Préserver les spécifications V1, worktrees/stashes et preuves. Vérifier l'absence de session concurrente.

Données normatives : 26667 samples/s x 16 B = **426672 B/s** ; campagnes V1 de 60 s à 604800 s. H3h-C2c : **1 bit / 10 MHz**, 524288 B écrits puis vérifiés, débit moyen **1202495 B/s** ; C2b : une latence de **298 ms sur 64 KiB**. Ces acquis sont des points de comparaison, non des garanties de pire cas ni une qualification endurance.

## Précondition physique impérative
Le rapport GPIO indique : microSD **retirée**, seconde Nucleo flashée avec GPIO PB15/PB3/PB4 **push-pull sorties basses**, SDMMC désactivé. **NE PAS insérer la microSD tant que le firmware GPIO est actif**, même à zéro. Codex doit préparer et flasher **d'abord** un firmware SD de démonstration sûr sur la seconde Nucleo identifiée, désactivant ces sorties et restaurant les broches en entrée/AF SDMMC appropriée. Ensuite seulement, demander à l'utilisateur d'éteindre le banc, d'insérer la microSD de test et de le remettre sous tension. Vérifier cette séquence avant tout accès. L'utilisateur assure les gestes physiques.

Seconde sonde historique ST-LINK 0032001F3235510C37333439 / UID 001B0046/38425018/20333157 ; **ne jamais flasher le TR2 principal** ST-LINK 003500463235510B37333439 / UID 004A003D. MicroSD du second banc : SanDisk Ultra 32 GB de test, **contenu entièrement sacrifiable selon autorisation explicite antérieure** ; contrôler CID/capacité et absence de confusion avec microSD principale. Pas de manipulations physiques autonomes.

## Exécution
1. Partir du démonstrateur isolé ST CubeU5 v1.9.0 `SD_ReadWrite_DMALinkedList` déjà porté dans `experiments/sdmmc_st_reference`. Tracer toute adaptation, vérifier les callbacks et DMA ; ne pas le présenter comme code ST natif Nucleo. Minimiser les modifications ; aucune modification du driver HAL officiel, aucun nouveau stockage propriétaire, pas de FRAM/IIS3DWB/Modbus.
2. Configurer **explicitement SDMMC2 1 bit**, et vérifier la largeur réellement appliquée (carte et CLKCR). Préférer une horloge déjà caractérisée de **10 MHz**, en contrôlant sa valeur effective ; utiliser un profil plus conservateur uniquement avec justification. Interdire toute tentative 4 bits dans cette mission.
3. Préparer un protocole progressif avec seuils explicites : vérification écriture/relecture de plusieurs blocs, puis ≥512 KiB, puis campagnes représentatives ≥60 s à **426672 B/s de données logiques**, puis endurance plus longue à durée et volume mesurés (viser ≥10 minutes et ≥256 MB si la capacité et le comportement le permettent ; documenter toute limite réelle). Couvrir séquences contiguës et tailles de chunks représentatives, y compris 64 KiB, sans écraser de données hors microSD de test. Aucune obligation de produire artificiellement 7 jours de données en temps réel dans cette tranche.
4. Mesurer débit moyen **et** distribution des latences (max, P95, P99, histogramme ou quantiles, temps d'attente carte TRANSFER), erreurs HAL/SDMMC/CMD13, éventuelles pauses et récupération ; conserver traces horodatées et compteurs. Séparer mesure brute du bus et scénario simulant une arrivée constante de 426672 B/s, avec tampon borné, backlog maximal et overflow observé, sans prétendre qualifier l'acquisition IIS3DWB. Un buffer infini ou un débit moyen seul ne valide pas TR2.
5. Contrôler intégrité de bout en bout avec motifs déterministes distincts, relecture et comparaison (au moins checksum cryptographique ou octet par octet des volumes concernés), vérification après reset/re-init, et test d'erreurs sans retry masqué. Préserver la traçabilité LBA, CID, zones et volumes réellement parcourus. La carte de test est effaçable/repartitionnable si nécessaire, mais ne pas le faire sans utilité démontrée.
6. Procéder de façon autonome à builds, flash/debug sur la seconde Nucleo, exécution et diagnostic logiciel raisonnable. Ne pas demander d'approbations répétitives pour opérations déjà autorisées. Si une intervention physique devient nécessaire, la décrire précisément. Ne pas introduire de modifications empiriques en cascade.
7. Comparer les mesures aux 426672 B/s requis et aux observations H3h-C2b/C2c, sans extrapoler la fiabilité à 7 jours. Évaluer capacité/buffering requis selon latences observées, limites et prochaine tranche. **Ne pas modifier ni reflasher le TR2 principal**, ni assouplir les gels V1.

## Critères d'acceptation
- Démonstrateur indépendant traçable, configuration 1 bit réelle prouvée et sécurité de transition GPIO→SD respectée.
- Mesures physiques répétables de volumes/cadences significatifs, intégrité, distribution de latences, backlog et marges ; ou BLOCKED motivé si l'essai ne peut être mené.
- Rapport séparant preuve du banc, simulation du flux TR2 et qualification industrielle restant à faire ; pas de fausse validation d'endurance ou de capacité V1.
- Commits/push normaux sur main, preuves vérifiables et `DERNIER_RAPPORT.md` publié, contrôlable indépendamment par ChatGPT.

## Garde-fous
`allow_commit: true`, `allow_push: true`, `allow_flash: true`, `allow_debug: true`, branche main. Flash uniquement seconde Nucleo identifiée, sans mass erase/Option Bytes/RDP ; microSD test sacrifiable uniquement, jamais le média principal ; pas d'écriture sur FRAM. Arrêt en cas d'identité incertaine, risque électrique ou session concurrente. L'utilisateur réalise les gestes physiques. Ne pas demander d'oscilloscope.

## Suite
ChatGPT examine indépendamment le rapport et les preuves avant décision sur l'intégration du mode 1 bit au stockage de production.
