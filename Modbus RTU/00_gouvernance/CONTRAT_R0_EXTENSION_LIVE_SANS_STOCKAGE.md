# TR2 — Extension normative LIVE sans stockage de campagnes (R0)

**Statut : contrat R0 adopté pour conception et validation du successeur ; non implémenté, non qualifié.** Décision utilisateur du 15/10/2026. Référence : audit A-06, étude `ETUDE_A06_SUPERVISION_SANS_STOCKAGE.md`. La V1 gelée et V1.1 transaction_epoch restent inchangées. Les valeurs proposées ci-dessous constituent le profil cible LIVE, à contrôler contre l'intégralité du mapping avant émission du mapping successeur.

## 1. Invariants et périmètre
- Acquisition IIS3DWB, calcul RMS/crêtes/alarmes, B3, Modbus, PC, SQLite et Web **sans média de campagnes**, avec autorité de configuration FRAM valide. Pas de campagne implicite, ni de XYZ durable, ni de store-and-forward promis.
- L'enregistrement de campagnes reste une fonction distincte, réintégrable après qualification du stockage. Une défaillance du stockage ne doit pas invalider une mesure live intrinsèquement valide ; les défauts détectés restent visibles.
- Préserver V1 : adresses, types, endianness, tailles, CRC historique, commandes 3/4, anciens tests et archives. Aucune nouvelle sémantique n'est attribuée rétroactivement à V1.
- Un seul propriétaire du flux capteur/SPI/FIFO ; pas de seconde acquisition lors de la réintégration de l'enregistrement. Acquisition arrêtée pendant APPLY. Pas d'auto-START au boot ni après réinsertion SD.

## 2. Contrat fonctionnel successeur
| Domaine | Décision R0 |
|---|---|
| B0 | Nouvelle version explicite distincte V1/V1.1 ; bit capacité 4 candidat LIVE_WITHOUT_CAMPAIGN_STORAGE. Les bits 0–3 gardent leur signification ; support SD ≠ disponibilité du média. Valeur numérique de version à réserver après vérification des tables de version. |
| B4 | Profil LIVE candidat `acquisition_mode=2, storage_mode=0, storage_limit_mb=0`. Profil campagne V1 `(1,1,limite positive <= capacité réelle)` inchangé. Refus de tout tuple mixte. `config_structure_version` nouvelle valeur explicite, à allouer après vérification des registres ; vérification hors CRC. |
| B4 CRC | Algorithme et périmètres inchangés : préparé 168 octets 4016..4099, actif 152 octets 4100..4175 ; vecteur V1 `0x5207CCFC` préservé. Nouveaux vecteurs live requis. Pas de publication active avant commit FRAM réussi. |
| FRAM | Nouveau format de record versionné avec lecture explicite des anciens records. Ne jamais convertir silencieusement campagne→live. Aucun slot supposé libre ; analyser layout, coupure, migration et rollback avant écriture. |
| B5 | START_CAMPAIGN/STOP_CAMPAIGN codes 3/4 conservés. START_LIVE/STOP_LIVE codes candidats 13/14, paramètres nuls ; code 12 réservé à RENEW_TRANSACTION_EPOCH V1.1. Transaction/idempotence et refus sûrs conservés ; codes et priorités de refus à vérifier avant gel du mapping. |
| B1 | Acquisition active possible sans stockage disponible ; `active_campaign_id=0` en live ; absence volontaire SD ≠ faute SD. Défaut SD réellement observé exposé, sans bloquer arbitrairement le calcul. |
| B3 | Indicateurs réels, validité/fraîcheur/âge issus des fenêtres réelles, jamais de zéro de remplissage valide. Bit8 signifie « campagne durable active » dans le successeur, donc 0 en live. |
| B6 | Aucune campagne créée/incrémentée par live. Statut santé candidat 4 = inventaire indisponible/non caractérisé, distinct d'un inventaire réellement vide ; compteurs 0 sous ce statut non interprétables. Révision de version B6 si requise. |
| B7 | Distinguer absence prévue, défaut actif, défaut mémorisé, acquittement et défaut FRAM. Pas d'autotest SD bloquant quand le mode LIVE n'en dépend pas. |
| PC/SQLite | Détection explicite de versions/profils/capacités supportés ; ancienne version PC refuse proprement nouveau firmware ; nouveau PC supporte explicitement les anciens profils. Historiser version, configuration, mode, séquence et qualité ; trous et doublons non déguisés en mesures valides. |

## 3. Supervision continue
**Décision R0 : supervision live de durée indéfinie en régime nominal**, avec arrêt explicite, arrêt sûr sur défaut bloquant et **pas de redémarrage automatique après reboot**. La borne `campaign_duration_s` V1 (60..604800 s) ne doit pas imposer l'arrêt de la supervision live. La représentation de la durée live (champ ignoré avec valeur canonique compatible, ou nouveau domaine versionné) et son comportement au rollover doivent être spécifiés et testés avant implémentation ; ne pas imposer arbitrairement un nouveau codage sans analyse. Les campagnes historiques restent bornées comme V1.

## 4. Exclusion, états et fautes
- Première version : live et campagne mutuellement exclusifs. START live avec configuration campagne, START campagne avec configuration live, START concurrent ou APPLY en acquisition = refus sans effet. STOP idempotent selon contrat B5 versionné, sans masquer un défaut.
- Média absent en live : normal et signalé indisponible, sans inventer une campagne. Panne réellement détectée : diagnostic persistant conformément aux règles d'acquittement ; un défaut FRAM/mesure peut bloquer le live, un défaut SD seul ne le bloque pas.
- B1/B3/B6/B7 doivent partager une table d'états explicite : boot, sans configuration, prêt, live actif, campagne active, arrêt, SD absente, SD en panne, FRAM corrompue, capteur défaillant, coupure/reprise ; préciser priorité des refus et sévérité sans inventer d'anciens oracles V1.

## 5. Critères de clôture normative R0
1. Publier **un mapping successeur distinct** avec versions numériques non conflictuelles, allocation effective des bits/commandes, domaines, réservés et exemples CRC ; préserver mappings et vecteurs V1/V1.1.
2. Établir une table normative exhaustive des états et refus croisés, et une politique d'alarme/validité/fraîcheur/temps et continuité live.
3. Définir schéma FRAM, migration, downgrade/rollback et power-loss, avec preuve d'absence d'écrasement de records anciens.
4. Décliner tests successeur (FT-LIM, CMD, INT, BLK, SEQ, PER, OBS, PC) à partir de la matrice A-06 ; ne modifier aucun gel historique.
5. Effectuer la revue de cohérence indépendante avant autorisation R1. **R0 n'est pas un PASS logiciel ou physique.**

## 6. Arbitrages actés et questions d'ingénierie
**Actés :** découplage, profil versionné, commandes distinctes, FRAM autoritative, supervision continue, exclusion initiale des campagnes, absence SD non bloquante, préservation V1 et compatibilité explicite.

**À résoudre par analyse technique dans R0 :** numéros définitifs version/capacités/commandes ; champ durée live ; gestion précise des priorités de refus, idempotence, alarme, auto-test et statuts ; stratégie de slots FRAM/rollback ; couverture des oracles successeur. Aucune décision ne permet une valeur réservée non vérifiée.
