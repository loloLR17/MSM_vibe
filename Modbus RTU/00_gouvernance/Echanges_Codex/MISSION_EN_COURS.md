---
mission_id: TR2-20261017-R5A-PC-LIVE-PROFILE-SQLITE-WEB
status: READY
base_ref: main
base_sha: d0a37efb7f29d6fb8ff4e275e6f666916ff79abb
created_at_utc: 2026-10-17T11:10:42.991Z
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# R5-A — Première intégration supervision PC du profil LIVE

## Objet, état de départ et limite de la tranche

R4-V est `DONE` **dans sa portée physique bornée**, avec échanges archivés à l'identique sous `archives/TR2-20261017-R4V-RTU-START-STOP-LATENCY/`. La chaîne STM32 IIS3DWB → datation événement → RMS/crêtes B3 N4096 → UART/RS-485 maître T1000 a déjà donné des valeurs physiques et des réponses CRC conformes. R4-V a caractérisé la latence B5 ; effet STOP mesuré à ~33,0 s après capture et terminal à ~40,9 s sous l'optimisation de banc. Ce n'est ni une exigence produit de STOP instantané, ni une qualification industrielle. Le chemin de sécurité physique et la causalité transactionnelle restent prioritaires ; l'optimisation n'a pas modifié R2/B5 gelés.

**But R5-A :** adapter le socle PC .NET/S0–S7 existant au décodage **lecture seule** du profil LIVE protocole 2, faire circuler des observations B3 qualifiées dans le domaine, la persistance SQLite et une première présentation Web explicite, d'abord par fixtures et transport simulé. Maintenir le produit PC V1 existant et ses acquis sans refonte. Cette tranche est **logicielle PC** : aucune écriture matérielle ni nouveau flash/debug STM32. Réserver la connexion bout en bout T1000/COM4 au jalon ultérieur R5-B/R6 après préparation et identification du véritable firmware LIVE ; le microSD/R7 ne bloque pas R5.

## Références et exigences non négociables

Lire le `main` réel, `AGENTS.md`, `GOUVERNANCE_CHATGPT_CODEX.md`, `PROTOCOLE_ECHANGE_V2.md`, état courant, roadmap R0–R7, profil normatif `01_Specification_source/LIVE_R0/PROFIL_LIVE.md` et mapping LIVE, V1/V1.1 B5 gelés, R4-Q/T/U/V et preuves (en particulier les captures ADU physiques). Examiner `Modbus RTU/Supervision TR2/` et ses documents S0–S7, tests, architecture .NET et SQLite actuelle ; ne pas remplacer les abstractions existantes par un second logiciel. Vérifier branche HEAD et statut R4-V DONE/archives avant toute modification.

**Sélection stricte du profil :** distinguer protocol_version 1 (V1 existante) et 2 (LIVE) par B0 réel, longueur/structure/capabilities documentées, sans inférer qu'un protocole 1 prend en charge les commandes epoch. Une image de qualification R4-U/V annonce protocole 2 et un sous-ensemble mais **n'annonce pas bit LIVE16** et laisse B1/B2/B7 non pris en charge : traiter cela comme une cible partielle/diagnostique, pas comme un produit LIVE qualifié. Ne jamais déclencher des écritures B4/B5 par détection automatique, ne pas inventer de capacités ni présenter une absence de données comme une acquisition valide. Les clients V1 demeurent compatibles et sûrs.

**Sémantique B3/temps :** décoder exactement les 48 registres (types, ordre MSW/LSW, unités), V/F, INVALID/FRESH/HELD, statut, âge, séquence, UTC absente, génération/configuration seulement lorsqu'observables/contractuelles ; interdiction d'attribuer artificiellement un run/epoch à un B3 si non exposé sur le fil. Distinguer horodatage réception PC, temps événement STM32 qualifié ou indisponible, qualité numérique, stale/timeout, reconnect et nouveau segment d'observation. Ni les trous ni les duplications au reboot/saturation/rupture COM ne doivent être dissimulés comme exactly-once.

**Idée à conserver, non normative à ce stade :** si les latences deviennent fonctionnellement problématiques, évaluer plus tard quelles commandes sont utilisables durant l'acquisition et lesquelles nécessitent l'état arrêté, au-delà des restrictions déjà existantes dans R0 et V1.1. **Ne pas changer V1.1/R0 ni les règles d'admission côté firmware dans R5-A.** Le PC devra néanmoins refléter les préconditions documentées, et distinguer acceptation, exécution, résultat durable et refus lorsqu'une couche future de pilotage sera développée. Aucun délai fixe arbitraire de 10 s faisant passer une transaction valide à FAILED.

## Exécution autonome par incréments

1. **Audit d'écart orienté résultat.** Cartographier les classes transport, detection B0, décodage registres V1, domaine, persistance, migrations SQLite, API Web et UI S7. Confronter avec les réponses physiques R4-U/V et les scénarios R0-PC ; lister champs observés/absents, limitations des images de banc et plan de migration additive. Définir un petit contrat de données LIVE et un lecteur strict explicite, sans casser les enregistrements historiques.
2. **Lecture/décodage PC.** Implémenter un décodeur LIVE protocole2 et sélection de profil sûre, avec tests sur captures ADU authentiques et fixtures normatives, CRC/longueur, états B3 incohérents ou réservés, endianess, unités, age/sequence/flags et qualité. Refuser ou signaler clairement les sous-ensembles de banc non habilités comme produits ; permettre une voie diagnostic explicitement marquée, lecture seule, si utile à R5-B, sans en faire une acceptation silencieuse.
3. **Modèle et SQLite.** Étendre de manière compatible l'observation persistante : identifiant appareil, profil/versions, contexte configuration connue ou inconnue, segment de session PC, temps réception PC et UTC STM32 distincte/qualifiée, état et qualité, valeurs/vibrations, séquence/âge, perte de communication, distinction doublon/trou/indéterminé. Migrations additives et tests ouverture d'anciennes bases, redémarrage/reconnexion, refus/corruption/SQLite indisponible ; aucune attribution rétroactive d'identité à d'anciennes lignes, aucune valeur invalide enregistrée comme validée. Ne pas utiliser campaign_id comme une fausse identité LIVE.
4. **API/Web S7 adaptée.** Mettre à disposition des données LIVE clairement étiquetées, leurs unités et la qualité/fraîcheur, mode arrêt/attente, UTC non synchronisée, perte/retour transport et limites de provenance. UI simple intégrée au socle Web existant, pas une refonte cosmétique. Les écrans ne doivent pas afficher une valeur HELD comme une valeur courante ni extrapoler des mesures non reçues. Tester contrats API, ancienne UI et vues nouvelles.
5. **Validation sur T1000 sans cible modifiée.** `dotnet restore/build/test` ou commandes adaptées aux SDK réellement disponibles, tests unitaires/intégration sur SQLite temporaire, fixtures réelles figées R4-U/V et simulations de coupures/reboot/profils mixtes ; tests négatifs de non-écriture via lecture/détection. Si l'environnement T1000 permet un démarrage Web local et smoke-test non destructif, l'exercer et publier la preuve ; sinon exposer limite explicite. Aucun accès COM4 nécessaire ni acquisition physique/flash/FRAM, donc ne pas revendiquer R6.
6. **Livraison et interface de reprise.** Documenter la matrice profil/champs et les capacités réellement acceptées ; note technique R5-A avec commandes de tests, captures de sorties, migrations et démonstration Web/API si possible. Préparer R5-B : raccordement au vrai transport COM4 partagé par un seul maître, puis firmware LIVE complet/B1/B2/B7 séparément ; ne pas présumer cette composition physiquement présente. Mettre à jour uniquement les **constats factuels** d'état courant/roadmap, préserver le gel documentaire, S0–S7 et les backups/worktrees/stashes.

## Critères d'acceptation et arrêt

`DONE` seulement si le profil LIVE est choisi sans confusion V1, les vrais ADU B3 sont décodés et testés, la qualité non valide/fraîcheur et UTC absente sont respectées, SQLite reste compatible avec les données historiques, l'API/Web montre honnêtement les mesures et limites, et les tests `.NET`/migrations sont réellement PASS. Les fonctions non réalisables faute de données exposées doivent être explicitement marquées inconnues/non disponibles, avec tests de refus, sans inventer un registre. Si un écart normatif impose une décision structurante, isoler ce point et signaler `BLOCKED` plutôt que modifier V1/V1.1/R0. Respecter A-06 : correction minimale, aucune refonte injustifiée.

Codex conduit code/tests/commits/push sur `main` avec `allow_commit: true`, `allow_push: true`, **`allow_flash: false`, `allow_debug: false`** pour cette mission PC sans STM32. Publier preuves et `DERNIER_RAPPORT.md` selon protocole, vérifier GitHub à distance et préserver l'historique des échecs ; le contrôle indépendant appartient à ChatGPT. Aucune validation humaine répétitive demandée. Si partie UI ou SQLite non complète, livrer un état `IN_PROGRESS` reprenable ou `BLOCKED` motivé sans annoncer R5-A complète.
