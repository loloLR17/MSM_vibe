# État courant MSM TR2 — synthèse opérationnelle

**Actualisation :** 13 octobre 2026, consolidation documentaire A-02. **Source de vérité :** branche `main` de `loloLR17/MSM_vibe`.

> Ce document est une synthèse opérationnelle, non un journal d'essais. Le document intégral antérieur est conservé **sans modification** dans [Archives/ETAT_COURANT_TR2_AVANT_A02_20261013.md](Archives/ETAT_COURANT_TR2_AVANT_A02_20261013.md). Il s'agit d'un état reconstruit à partir des preuves disponibles, et non d'une restitution du journal HP historique introuvable.

## 1. État matériel et logiciel

- **Cible principale :** TR2-001 / NUCLEO STM32U575 ; capteur IIS3DWB sur SPI3. Firmware **cumulatif M014+M015** conservé sur la carte après l'essai réussi M015-V ; CPU 160 MHz, SPI3 4 MHz mode 3. Ne pas restaurer automatiquement M014-H.
- **Code M015-V déclaré testé par Codex :** `ef468539b9943cc9b9b4d832e513a0f8ab05c7ae` ; commit documentaire de preuves `92c0966d5a4f84ee4033490825e3f7cc6dc6737a`. SHA-256 BIN : `8529630a5a6c9aaa2449fde512b06d6ca16a89a08b7afea2abc9ecca3b75d480` (rapport M015-V). **Revue indépendante A-01 et tag `tr2/M015` encore à réaliser.**
- **Supervision PC :** socle S0–S7 développé ; M014-H a démontré sur le TR2 réel les lectures B0/B1, le polling et la perte/reprise dans une portée bornée. Pas de preuve de mesure vibratoire V1 affichée.
- **STM32/Modbus :** B0/B1 fonctionnels dans l'essai M015-V ; correction de préparation B5 inutile sur lecture FC03 B0/B1. B5 avec accès FRAM reste potentiellement bloquant et non coqualifié avec acquisition.

## 2. Preuves acquises et limites

- **M015-V — mission `DONE`, acquisition physique bornée :** une capture continue de **32 768 XYZ + 4 096 timestamps**, préfixes 4 096/8 192/16 384/32 768, durée environ **1,247 s** ; 304 bursts, FIFO max 158, aucun FULL/OVR observé sur essai corrigé, nettoyage final conforme. Deux lectures Modbus B0/B1 durant l'acquisition, sans erreur UART signalée.
- Contrôle RAM/CRC par bursts concordant ; bruts SPI intégraux reconstruits seulement pour les derniers 4 096. Une première tentative échouée (pause ~141 ms, FIFO FULL/OVR) est conservée séparément ; correction de la reconstruction B5 non demandée étayée par comparaison.
- **Non qualifiés :** perte globale zéro, endurance, calibration/métrologie, FFT/indicateurs, stockage de campagne V1, SDMMC 4 bits, coexistence B5/FRAM/SD avec acquisition, publication vibratoire V1 et chaîne capteur→Web complète. Les tests hôte/ARM ne valent pas preuve physique.
- **Jalons :** M014 `DONE` dans sa portée ; M015 `DONE` dans sa portée d'acquisition bornée selon Codex, revue indépendante A-01 en attente ; **J3 EN COURS** ; M016–M020 à faire.

## 3. Préservation et reprise

- FRAM : génération 12 et domaine actif préservés dans les essais rapportés ; sauvegardes M014-H vérifiées et réutilisées. MicroSD non initialisée/accédée durant M015-V. Option Bytes non modifiés selon procédure rapportée.
- Carte laissée avec firmware cumulatif, capteur au repos après cleanup, débogueur détaché. Vérifier **l'état réel actuel** avant toute intervention physique ultérieure ; ces indications sont celles du rapport M015-V, pas une télémétrie en direct.
- Préserver les backups FRAM/Flash, l'identité, les médias et les archives de tests. Opérations physiques à l'utilisateur ; Codex peut flasher/déboguer dans les limites de `AGENTS.md` et de la mission.

## 4. Prochaine étape

1. Finaliser la consolidation documentaire **A-02** ; appliquer les règles A-01 (fiche de qualification et tag uniquement après revue indépendante).
2. Auditer avant M016 la **configuration STM32 cumulative reproductible**, les budgets temporels FIFO/Modbus/FRAM/SD, les drivers officiels ST et la couverture fonctionnelle des tests (**A-04/A-05**). Corriger uniquement les risques significatifs selon A-06.
3. Reprendre **M016** : traitements et publication des vibrations dans le mapping Modbus V1 gelé, puis M017–M020 selon la roadmap.

## 5. Références durables

- [Roadmap](ROADMAP_TR2_VERS_SUPERVISION_VIBRATOIRE.md) : jalons et dépendances.
- [Rapport Codex M015-V](Echanges_Codex/DERNIER_RAPPORT.md) : tests, empreintes, preuves, limites et publications.
- [Historique complet pré-A-02](Archives/ETAT_COURANT_TR2_AVANT_A02_20261013.md) : missions M013 à M015-V et reconstruction T1000 ; préservé byte-identique.
- [Gouvernance ChatGPT](GOUVERNANCE_CHATGPT_CODEX.md), `AGENTS.md` à la racine, et fiches A-01 dans `Modbus RTU/02_Validation/Qualifications/`.
