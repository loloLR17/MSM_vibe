---
mission_id: TR2-20261011-M013-A-TX-PREFLIGHT
status: BLOCKED
base_ref: main
base_sha: null
initial_head: 81138a6f996d1e3ba9c26620f6d4b3b22b844a8a
mission_sha: 6a35aefdef34b91ed82d217c37cabc4a04bf2e8f
result_sha: 184b31550e0ce52d28220145967db57ba6fc399b
created_at_utc: 2026-10-11T13:44:35.966396+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M013-A — diagnostic FC03 livré ; preflight physique NO-GO

**TRANCHE LOGICIELLE VALIDÉE / 108 TESTS HÔTE / ARM DÉFAUT ET DIAGNOSTIC OFF-ON COMPILÉS / AUCUN FLASH OU TX PHYSIQUE.** BLOCKED concerne la qualification preflight physique : VIO Click et absence de commande concurrente via /RE–J3–RST restent non établis. Première publication/relecture distante également à obtenir pour ce rapport provisoire. Le statut ne sera pas passé DONE sur la seule réussite de cette publication.

## Références, inspection et préservation

HEAD initial propre `81138a6f996d1e3ba9c26620f6d4b3b22b844a8a`, main. Fetch puis pull fast-forward vers `6a35aefdef34b91ed82d217c37cabc4a04bf2e8f`, seule mission active changée. Contrat M013-A READY, target main et quatre permissions true. Champs base_sha/created_at absents du contrat : référence observée consignée, base_sha null ; fichier ChatGPT inchangé. AGENTS.md et journal lus intégralement, protocole V2-B3, rapport012, audit012, documents câblage008/007 et pinout historique, B1 normatif, sources et tests concernés inspectés. Sources MIKROE/ADI relues ; PDF Click ouverture web en erreur, graphique local008 intégral relu et empreinte sauvegardée, sans prétendre un nouveau téléchargement.

Archives012 mission/rapport copiées exactement depuis `81138a6…`, sans écraser d'archive. Deux stashes005/002 conservés. Aucun freeze V1, configuration permanente, raccordement matériel ou travail E4 modifié. Aucun ancien travail refait.

## Modifications et choix architectural

Commit évalué **`184b31550e0ce52d28220145967db57ba6fc399b`**, message **`STM32: isolate FC03 system diagnostic behind explicit bench gates`**.

[Matrice et analyse détaillée](../PREFLIGHT_RTU_TX_M013_A.md) : conditions prouvées avec provenance/date, lacunes, risques et moyen minimal. Les mesures3,31V MCU, SB28/SB29, pontJ2 soudé rapporté, câblage PG4 et RX011 ne sont pas redemandées et ne deviennent pas une preuve VIO/RST/TX.

- `platform/stm32/stm32_modbus_diagnostic.{c,h}` : branche de diagnostic distincte, vrai RTU/PDU/CRC/filtres existants. Seule lecture normale disponible FC03/1000/quantité1 → **UNKNOWN=0**, initialisation incomplète, conforme B1§6.1. Aucun runtime prêt fabriqué, B0/température/charge/stockage/flags inventés. Tout autre accès à une donnée valide mais non disponible passe par exception04 existante ; FC16 RO/autorité absente et fonctions non supportées conservent les exceptions V1. Toutes autorités de commande/time/configuration/campagne NULL ; broadcasts sans réponse, aucun retry.
- `include/tr2/modbus/rtu_server_runtime.h`, `src/modbus/rtu_server_runtime.c` : callback optionnel `prepare_request`, après acceptation CRC/adresse et avant PDU, vue empruntée synchrone. Défaut NULL préserve la composition existante. Nécessaire pour n'exposer que le registre réellement connu, sans dupliquer RTU ni transmettre un B1 rempli de valeurs fictives. Ancien callback prepare et readiness production conservés.
- `platform/stm32/main.c` : diagnostic sélectionné au build avant SPI/capteur/FRAM/SD/boot production ; boucle dédiée, compteurs distincts de disponibilité/poll/erreurs. Aucun média touché dans cette branche, aucun format ou readiness production forcé. Le chemin par défaut reste le harness existant.
- CMake STM32 : diagnosticOFF et TXOFF par défaut, adresse diagnostique explicite1..247 obligatoire (défaut0 refusé si diagnosticON). Candidats à unité1, marqueur de banc du probe006, pas adresse production provisionnée. CMake hôte et tests plateforme nouveaux/étendus.
- Journal actualisé avec diagnostic, validation, NO-GO, prochaine intervention unique et image008 conservée ; archives012 et document M013-A.

**Binding production toujours NOT_AVAILABLE.** Une véritable composition exige transport/adresse qualifiés, dépendances durables, géométrie FRAM valide, récupération transactionnelle valide, CampaignDataStore réel et environnement/métadonnées pertinents. EMPTY refuse sans format. B0 est optionnel dans l'API mais non provisionné ; sans identité, lecture04. Production readiness n'est pas levée. Le diagnostic permet le futur essai minimal sans inventer ces autorités ; il ne qualifie pas le capteur industriel.

## Validations réellement exécutées

Validation complète retenue selon AGENTS.md§6 : changement d'interface RTU partagée, risque de régression plausible.

```sh
STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 bash ./tr2_validate.sh
```

Retour0 : **108/108 tests hôte et cross-build STM32 défaut réussis**. Après ajout des assertions de raccordement diagnostic/transport uniquement, `cmake --build 'Modbus RTU/05_Firmware/build-host-validation' --parallel 4` puis `ctest --test-dir 'Modbus RTU/05_Firmware/build-host-validation' --output-on-failure` : retours0, **108/108** (3,48s). Sources firmware évaluées identiques aux cross-builds après ces seules extensions de tests. Aucun test supprimé/affaibli ou résultat attendu ajusté pour masquer un défaut. Pas de test .NET, aucune source PC modifiée.

Deux configurations réellement exécutées par subprocess, source `Modbus RTU/05_Firmware/platform/stm32`, CMake/Ninja, toolchain `cmake/arm-none-eabi-gcc.cmake`, SDK Linux v1.9.0, `TR2_STM32_MODBUS_DIAGNOSTIC=ON`, `TR2_STM32_DIAGNOSTIC_UNIT_ID=1` : `TR2_STM32_RS485_TX_QUALIFIED=OFF`, puis `ON`, dossiers séparés hors Git ; configure/build `--parallel 4` retours0. Configure diagnosticON sans adresse fournie : retour non nul et message attendu vérifiés, refus de sécurité volontaire. Quatre warnings newlib habituels `_close/_lseek/_read/_write`, pas d'erreur de compilation.

Tests : UNKNOWN et CRC, non-exposition B1 entier/température/B0, écritures B1/B2/B5 refusées, FC06, foreign/broadcast/CRC invalide, aucun TX avantT3.5, défautTX sans replay. Tests réels diagnostic→RTU/PDU→adaptateurSTM32 sur HALsimulée, requête dorée `01 03 03 E8 00 01 04 7A`, réponse dorée `01 03 02 00 00 B8 44`, gateOFF/ON et maintienDE jusqu'àTC puisRX. Couverture012 préservée. `arm-none-eabi-nm` et `objdump --disassemble=main` confirment l'absence des fonctions FRAM/SD/boot production dans les ELF diagnostic et la boucle dédiée ; lecture statique, pas preuve cible.

BIN/ELF, longueurs et empreintes complets dans document M013-A et manifeste local `artifacts.json`. Candidat diagnosticTXON : BIN48240 octets SHA256 `47d69cc286b0f12771de56eb80f2648cc3c2a7a57b2ce1a8038b34de792b97e8`, ELF SHA256 `762dcf0ba6c3663d9737dd334118a9f7d52a22175dda6d2674e9cb0d7b8e1c4a`. **Non flashé, non qualifié matériellement.** Preuves013 hors Git `/home/lolo/dev/msm/reports/TR2-mission013-A-20261011T133639Z` : état initial, manifeste index, logs validation/tests/builds, refus adresse, graphique/PDF identifié, symboles, disassemblages.

## Matériel, limites et intervention attendue

Aucune connexion MCU/flash/debug/reset/USB/écriture de registre/émission013. Permissions flash/debug présentes mais non exercées : un nouveau halt/registre MCU ne mesure ni VIO ni route RST et ne lèverait pas le blocage. Pas de nouvelle sauvegardeFlash nécessaire sans flash ; lors du futur flash, sauvegarde2Mio de l'état alors courant et vérification/readback obligatoires. Dernière identité physique établie011 : BIN008157944 octets SHA256 `02ed2cf64d4d78225a4781b823c9d27e96a8ea693679d8a834d230568383593b`, ELF008 `6546629b85dba6ba5ead029d208d9a95e67a13e0e244d477d01828b07ab4c23e`, copies horsGit012 rehashées013. Pas de symbole ELF013 interprété sur cette carte.

**Intervention physique unique, sans émission/recâblage/reset :** au repos sous BIN008, relever VIO(U1-7), DE(U1-10), /RE(U1-9) contre GND1 logique, sur points accessibles identifiés ; attendu VIO≈3,31V, DE et /RE≈0V et sous0,3VIO. Dans la même inspection, indiquer la révision lisible, étatJ3 et si RST du Click rejoint un fil/socket/autre sortie. Ne pas mesurer de continuité sous tension, ne pas court-circuiter des pattes fines ni créer une liaison GND1/GND2 ; point inaccessible/route inconnue → rapporter le manque. Motif précis : AGENTS.md§9 réserve le geste à l'utilisateur ; contrat§5 impose cet arrêt ciblé. Une tension incompatible ou une commande concurrente/inconnue maintient NO-GO ; des relevés conformes permettent une réévaluation, pas une qualification de transitions/reset automatique.

Étape suivante préparée dans M013-A : préflight sûr, contrat suivant, build identifié, sauvegarde2Mio/SN/flashstandard/readback/ELF correspondant, un échange HP COM7 1152008E1 sans retry et instrumentation DE/TXD/bus/TC/retourRX. Aucun lancement HP automatique et aucune répétition RX. Reset avant init et watchdogIRQ restent limites, E4 inchangée. À ChatGPT : revue indépendante de la tranche publiée, recueillir ce relevé unique puis cadrer le jalon physique. Hypothèses électriques non converties en faits.

## Git et publication

Arbre initial propre, `main`, remote GitHub vérifié ; origin/main restait `6a35aef…` au contrôle avant commit. Treize fichiers de la tranche indexés/relus, contrôle whitespace/secrets et archives exactes réussi ; aucun fichier hors périmètre. Permissions exercées : modification, tests/builds, documentation, commit ; push et relecture du présent rapport encore en cours. Aucun matériel ni opération destructive. État au commit code : arbre propre, ahead1 ; deux stashes préservés. Ce rapport sera publié normalement `HEAD:refs/heads/main`, puis tous les livrables seront relus depuis la référence distante récupérée. Le SHA final du rapport sera fourni séparément après vérification, sans auto-référence circulaire.
