# État courant MSM TR2 — synthèse opérationnelle

**Actualisation :** 13 octobre 2026, précondition métier M016 examinée ; décision nécessaire. **Source de vérité :** branche `main` de `loloLR17/MSM_vibe`.

> Ce document est une synthèse opérationnelle, non un journal d'essais. Le document intégral antérieur est conservé **sans modification** dans [Archives/ETAT_COURANT_TR2_AVANT_A02_20261013.md](Archives/ETAT_COURANT_TR2_AVANT_A02_20261013.md). Il s'agit d'un état reconstruit à partir des preuves disponibles, et non d'une restitution du journal HP historique introuvable.

## 1. État matériel et logiciel

- **Cible principale :** TR2-001 / NUCLEO STM32U575 ; capteur IIS3DWB sur SPI3. Firmware **cumulatif M014+M015 avec correction A04-02** conservé sur la carte après l'essai de coexistence ; CPU 160 MHz, SPI3 4 MHz mode 3. Ne pas restaurer automatiquement M014-H.
- **Code courant A04-02 exécuté par Codex :** `8bcb8436822ce7444074865617cb3696f877a6ad`, BIN169948octets SHA `b4301d6f3325c266edc19297413fd0885828e5625ec030a8acad340444b3fd9b`. **OBSERVÉ PHYSIQUE BORNÉ** :32768XYZ/4096TS,6readsHP sans retry, B5 retiré pendant capture/relu après cleanup ; qualification ChatGPT A04-02 acquise dans ce périmètre borné le 13/10/2026.
- **Code M015-V déclaré testé par Codex :** `ef468539b9943cc9b9b4d832e513a0f8ab05c7ae` ; commit documentaire de preuves `92c0966d5a4f84ee4033490825e3f7cc6dc6737a`. SHA-256 BIN : `8529630a5a6c9aaa2449fde512b06d6ca16a89a08b7afea2abc9ecca3b75d480` (rapport M015-V). **Revue indépendante A-01 et tag `tr2/M015` encore à réaliser.**
- **Supervision PC :** socle S0–S7 développé ; M014-H a démontré sur le TR2 réel les lectures B0/B1, le polling et la perte/reprise dans une portée bornée. Pas de preuve de mesure vibratoire V1 affichée.
- **STM32/Modbus :** B0/B1 fonctionnels dans l'essai M015-V ; correction de préparation B5 inutile sur lecture FC03 B0/B1. A04-02 retire B5 par exception04 avant I/O pendant capture ; réponse fraîche après cleanup. I/O B5 reste synchrone au repos (max144,138ms observé), boot bloquant non corrigé.

## 2. Preuves acquises et limites

- **M015-V — mission `DONE`, acquisition physique bornée :** une capture continue de **32 768 XYZ + 4 096 timestamps**, préfixes 4 096/8 192/16 384/32 768, durée environ **1,247 s** ; 304 bursts, FIFO max 158, aucun FULL/OVR observé sur essai corrigé, nettoyage final conforme. Deux lectures Modbus B0/B1 durant l'acquisition, sans erreur UART signalée.
- Contrôle RAM/CRC par bursts concordant ; bruts SPI intégraux reconstruits seulement pour les derniers 4 096. Une première tentative échouée (pause ~141 ms, FIFO FULL/OVR) est conservée séparément ; correction de la reconstruction B5 non demandée étayée par comparaison.
- **Non qualifiés :** perte globale zéro, endurance, calibration/métrologie, FFT/indicateurs, stockage de campagne V1, SDMMC 4 bits, I/O B5/FRAM/SD exécutée durant acquisition (A04-02 observe seulement son refus borné), publication vibratoire V1 et chaîne capteur→Web complète. Les tests hôte/ARM ne valent pas preuve physique.
- **Jalons :** M014 `DONE` dans sa portée ; M015 `DONE` dans sa portée d'acquisition bornée selon Codex, revue indépendante A-01 en attente ; **J3 EN COURS** ; M016–M020 à faire.

## 3. Préservation et reprise

- FRAM : génération 12 et domaine actif préservés dans les essais rapportés ; sauvegardes M014-H vérifiées et réutilisées. MicroSD non initialisée/accédée durant M015-V. Option Bytes non modifiés selon procédure rapportée.
- Carte laissée avec firmware cumulatif, capteur au repos après cleanup, débogueur détaché. Vérifier **l'état réel actuel** avant toute intervention physique ultérieure ; dernier contrôle A04-02 : cleanup0C8000/READY, CRCFRAM avant/après concordant et logiquegen12 byte-identique, UART sans erreur ; pas une télémétrie en direct.
- Préserver les backups FRAM/Flash, l'identité, les médias et les archives de tests. Opérations physiques à l'utilisateur ; Codex peut flasher/déboguer dans les limites de `AGENTS.md` et de la mission.

## 4. Prochaine étape

1. **A-02 achevée** et **A-03 harmonisée** ; réaliser la revue indépendante A-01 de M015 (fiche et tag uniquement si les preuves le permettent).
2. **A04-01/A04-02 qualifiés et statuts de dette actualisés le 13/10/2026** ; conserver les limites de budget/couverture/endurance, A04-04 à A04-08 et la revue A-01 M015 distincte.
3. Exécuter **M016** : traitements et publication des vibrations dans le mapping Modbus V1 gelé, puis M017–M020 selon la roadmap.

## 4 bis. Décisions après audit A-04

- Audit A-04 livré par Codex (`TR2-20261013-A04-AUDIT-FIRMWARE`, `DONE`) ; revue documentaire indépendante effectuée, **sans qualification physique nouvelle**. Le build cumulatif reproduit le SHA-256 BIN M015-V selon les preuves Codex.
- **Registre de dette A-04 :** [DETTE_TECHNIQUE_A04.md](DETTE_TECHNIQUE_A04.md), neuf constats ouverts ou différés, avec priorités et critères de clôture. Ne pas effacer ces constats lors du passage A-05.
- Ordre décidé : **A-05 tests et preuves → A04-01 build sûr → A04-02 FRAM/Modbus → A04-03 intégration M016**. A04-08 avant activation SD ; autres dettes suivies par ID.
- La mission A-04 est clôturée par son rapport `DONE` ; le contrat `READY` historique ne doit pas être relancé.
- **13/10/2026 — A04-01, VALIDÉ LOGICIEL par Codex :** sélection explicite des profils, refus du harness SD implicite, BIN cumulatif byte-identique à M015-V. [Rapport de correction](Echanges_Codex/archives/TR2-20261013-A04-01-SAFE-BUILD/RAPPORT.md), code `c9ee22292ae69cc6f333aaab3527d18556ee8272`. **Qualifié logiciel par ChatGPT le 13/10/2026 ; dette A04-01 clôturée dans cette portée** ; registre inchangé, aucun matériel accédé ni état actuel relu. A04-02 reste hors de cette correction.

- **13/10/2026 — A04-02, VALIDÉ LOGICIEL / OBSERVÉ PHYSIQUE BORNÉ par Codex :**126tests et cross-build ; FIFOmax293, FULL/OVR0, gapDWT5,855ms, réserve nominale7,30ms/marge1,445ms. B3 métier reste indisponible ; B5exception04 pendant capture, journal frais après cleanup. [Rapport archivé A04-02](Echanges_Codex/archives/TR2-20261013-A04-02-FRAM-MODBUS/RAPPORT.md). **Qualifié logiciel et physique borné par ChatGPT le 13/10/2026 ; dette A04-02 clôturée dans cette portée.** A04-03/M016 à exécuter.

## 5. Références durables

- [Roadmap](ROADMAP_TR2_VERS_SUPERVISION_VIBRATOIRE.md) : jalons et dépendances.
- [Rapport Codex M015-V](Echanges_Codex/archives/TR2-20261013-M015-V-COOPERATIVE-CLOTURE/RAPPORT.md) : tests, empreintes, preuves, limites et publications.
- [Historique complet pré-A-02](Archives/ETAT_COURANT_TR2_AVANT_A02_20261013.md) : missions M013 à M015-V et reconstruction T1000 ; préservé byte-identique.
- [Gouvernance ChatGPT](GOUVERNANCE_CHATGPT_CODEX.md), `AGENTS.md` à la racine, et fiches A-01 dans `Modbus RTU/02_Validation/Qualifications/`.


## Décision de pilotage du 13/10/2026

ChatGPT a qualifié A04-01 (logiciel) et A04-02 (logiciel + essai physique borné) sur rapports, commits et preuves publiés. La qualification ne suppose pas une reproduction indépendante de tous les tests. Rapports durables : `Echanges_Codex/archives/TR2-20261013-A04-01-SAFE-BUILD/RAPPORT.md` et `Echanges_Codex/archives/TR2-20261013-A04-02-FRAM-MODBUS/RAPPORT.md`. La qualification A-01 et le tag M015 demeurent distincts et en attente. M016 est la prochaine mission ; ne pas confondre DONE Codex et qualification future M016.

## M016 — 13/10/2026, précondition métier à arbitrer

- **VALIDÉ HORS LIGNE** : record de configuration conservé A04-02 CRC/décodage conformes (gen1/config1,±2g,4096,storage1/limit1MB). Le validateur C réel refuse l'environnement production `{false,0}` ; mode0/limite0 interdits parV1. Recovery UNAVAILABLE ne publie aucune ActiveConfiguration ; démarrage métier refusé.
- **BLOCAGE DE DÉCISION** : M016 sans SD/stockage disponible ne peut publier une mesure métier sous configuration autoritative avec les politiques actuelles. Choisir une voie de stockage explicitement attribué/qualifié, ou une évolution explicite de la politique de supervision sans stockage ; aucun contournement appliqué. Cartographie26champs/48registres et détails dans le rapport M016. A04-03 reste ouverte ; M016 non terminée.
- **NON VÉRIFIÉ sur cible pour M016** : aucune opération matérielle/Modbus/FRAM/SD/reset/flash/debug. Firmware conservé, dernier état documenté A04-02 inchangé ; pas une télémétrie actuelle. Aucun test/cross-build antérieur répété, aucun code fonctionnel modifié. Reprise après décision publiée, puis glueFIFO/configuration/fenêtres/validité/B3 et qualification bornée.

## Mission préalable stockage M016 — checkpoint du 13/10/2026

- **OBSERVÉ PHYSIQUE, LECTURE SEULE** : carteCID03534453/44353132/8562E43C/F80195EA, partitionMBR LBA65536 inchangée ; régioncandidate8..24Mio intégralement lue et sauvegardée (16Mio nuls). FRAM complète identiqueA04-02/gen12. Aucun writeSD/FRAM à ce checkpoint.
- **VALIDÉ LOGICIEL / PRÉPARÉ** :127tests et cross-build ; portmedia confiné, profilqualification isolé à déclenchementRAM, attributiondocumentée dans ALLOCATION_STORAGE_M016.json. BIN cumulatif original reproductible ; qualificationwrite/checkpoint/recovery/débit non effectuée. Carte actuellement en probe readonly, SDinitialisée1bit ; returncumulatif prévu après essais.
- M016-V1-VIBRATION reste BLOCKED et distincte. Mission STORAGE-PREREQUISITE IN_PROGRESS : campagne stockage bornée, puis état final et rapport avec limites, avant contrôle indépendantChatGPT.
