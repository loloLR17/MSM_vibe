# MSM TR2 — A-01 : revue indépendante R1-ARM-DSP-FIFO-BENCH

**29/10/2026 — responsable ChatGPT. Verdict : QUALIFICATION PHYSIQUE BORNÉE DU BANC R1-ARM TR2-002, NON QUALIFIÉ POUR PRODUCTION/STOCKAGE/ENDURANCE.** Ce jalon de banc ne reçoit pas de tag industriel `tr2/Mnnn` : il ne correspond à aucun jalon Mnnn complet et la coexistence avec FRAM/RTU/SD ainsi que l'endurance restent non testées. La décision tient compte d'un rapport Codex `DONE`, de ses limites et d'une revue indépendante des commits, des sources, du manifeste et de l'arithmétique de résultats ; **ChatGPT n'a pas personnellement commandé le matériel, ni répété les essais DWT, ni décompressé intégralement toutes les traces binaires**, et ne prétend donc pas à une contre-mesure physique.

## Identités, provenance et preuves

- Dépôt `loloLR17/MSM_vibe`, HEAD de `main` lu en ouverture de revue : `f0dc1133a6aa0f5c5a4f43f741d62dc5920a5c23`. Mission `TR2-20261029-R1-ARM-DSP-FIFO-BENCH`, contrat `READY`, rapport **du même identifiant `DONE`** : pas de relance de cette mission. Mission et rapport archivés **byte-identiques** dans `Echanges_Codex/archives/TR2-20261029-R1-ARM-DSP-FIFO-BENCH/`.
- Code FIR scientifique de référence inchangé : `0ad417d621c0023a2c326383040a5d4e73016329`. Firmware banc micro physiquement évalué selon preuves : commit `0c4b86997829496768bd31bf0fd9bf7b27dbdc0a`. Firmware banc FIFO physiquement évalué selon preuves : commit **`8c14f1037af2e82d252852cdbfcb184614135216`**, descendant de `962f9028` ; captures et rapport `71bf9d75ae52dadb0c25296bc841fbd545c9d939`, clôture documentaire `f0dc1133a6aa0f5c5a4f43f741d62dc5920a5c23`. Ces quatre commits et leurs parents/diffs ont été lus indépendamment via GitHub.
- Cible **TR2-002 exclusivement**, ST-LINK `0032001F3235510C37333439`, UID `001B0046 / 38425018 / 20333157` rapportés d'après lectures réelles, **distincts de TR2-001**. Un backup 2 MiB de la Flash initiale est attesté avec SHA-256 `dd1a2b44d155abbfc094dacf498d61f973688f83cf6c7685ecb265bc444fb1f1`. Deux flashs standards (micro puis FIFO) avec vérification, aucun mass erase/OB/RDP/SD/FRAM/COM4 selon rapport.
- Firmware FIFO BIN 34 192 B, SHA-256 `d30c8bd9b2ad0fc1c5b02982b8384b4866d7760a8e3d2c67c2391bfb5b2f868d`, ELF SHA-256 `898fc69da26543b13189b43ecd86df1a510e5df9c6b14743fc8387157cd76c9e`. Profil GCC ARM 14.2.1 / Cortex-M33 / O2 / soft-float, HCLK 160 MHz. Toutes ces identités de fichiers/binaires sont **rapportées par les manifestes Codex**, sans recalcul indépendant du contenu binaire complet.
- Sources indépendamment inspectées : `experiments/r1_dsp_arm_bench/fifo/main.c` (UID fermé sur 002, mesures DWT, commandes RAM conditionnelles, STOP avant capture, SPI3 unique) ; `experiments/r1_dsp_arm_bench/README.md` (protocole mesuré, absence de startup autorun) ; `Modbus RTU/05_Firmware/platform/stm32/fifo_event_time.c/.h` (sink opt-in exclusif, voie LiveMetrology par défaut préservée).
- [Rapport et limites](../../00_gouvernance/Echanges_Codex/archives/TR2-20261029-R1-ARM-DSP-FIFO-BENCH/RAPPORT.md), [preuves FIFO](../../00_gouvernance/Echanges_Codex/preuves/TR2-20261029-R1-ARM-DSP-FIFO-BENCH/fifo/README.md), [preuves micro](../../00_gouvernance/Echanges_Codex/preuves/TR2-20261029-R1-ARM-DSP-FIFO-BENCH/micro/README.md) ; [manifeste distant](../../00_gouvernance/Echanges_Codex/preuves/TR2-20261029-R1-ARM-DSP-FIFO-BENCH/REMOTE_REREAD_71bf9d75.json) attestant le contrôle par Codex de 146 fichiers et 100 pièces FIFO + 38 micro, sans le confondre avec une vérification binaire indépendante intégrale par ChatGPT.

## Résultats physiques rapportés et vérification arithmétique indépendante

| Critère — sessions distinctes | Bypass 2 s | FIR 2 s | FIR 10 s |
|---|---:|---:|---:|
| XYZ validés | 52 600 | 52 608 | 264 056 |
| Sorties `ceil(N/10)` | 5 260 | 5 261 | 26 406 |
| Service moyen/P95/P99/max (µs) | 633,831/650,100/677,231/**719,706** | 1285,016/1399,869/1438,900/**1455,119** | 1313,839/1399,750/1400,525/**1455,269** |
| Work maximum (µs) | 136,606 | 570,800 | 570,575 |
| Gap maximum (µs) | 973 | 1709 | 1709 |
| FIFO maximum (mots) | 29 | 51 | 50 |
| FULL/OVR/faute/clip détectés | Aucun déclaré | Aucun déclaré | Aucun déclaré |

**Contre-calculs ChatGPT :** `ceil(52600/10)=5260`, `ceil(52608/10)=5261`, `ceil(264056/10)=26406`; au pire du couple court/long, les marges arithmétiques aux références historiques `work 1000/service 2500/gap 5000 µs` sont **429,200 / 1044,731 / 3291 µs**, chacune sur son maximum propre, **non simultanément additionnable**. Ces marges représentent seulement les scénarios testés, et non un WCET démontré pour l'ensemble des stimuli/températures/cartes SD.

**Micro-benchmark apparié sur véritable DWT** : par émission `Q15 ≈9364 cycles` sur un bloc 60 contre `≈182 cycles` bypass ; 160 MHz. Le coût brut extrapolé au débit nominal donne `~24,064%` de temps instrumenté Q15 contre `~3,616%` bypass pour 60 sources, **sans prétendre mesurer directement la charge globale CPU**. Un bloc **128** peut dépasser 1 ms de work ; le protocole FIFO choisit **un quantum borné de 64 mots** et mesure de nouveau les maxima, sans relever les limites. Les deux jeux physiques bypass/FIR ne reçoivent pas des excitations strictement identiques : la comparaison de la chaîne FIFO est empirique et **non** une mesure à stimulus identique garantie ; seule la partie RAM micro est réellement appariée.

**Chaîne capteur :** WHO_AM_I réel `0x7B`, `CTRL1_XL=0xAC` correspondant au choix logique ±8 g, *sans étalonnage ni essai machine ±6 g*. Reconstruction DEC8 au µs d'après anchors, résidus 5/8/11 µs pour les fenêtres citées ; 103 sorties des deux sessions FIR vérifiées hors cible sur leurs 1024 premiers XYZ. Les sorties suivantes sont **contrôlées par compteurs/hash et absence de défaut déclaré**, pas par archivage intégral ni revue indépendamment chaque échantillon. STOP confirmé et capteur arrêté ; 4/4/8 XYZ restants sans prochaine ancre explicitement non émis.

**RAM/stack :** BSS du profil bench **731 416 B** (l'essentiel est instrumentation), pile marquée 4 Kio et profondeur observée **816 B** sur les fenêtres ; pas une marge SRAM de produit ni une borne de pile exhaustive. Les volumes de SRAM, buffers SD et transferts futurs doivent être mesurés sur une autre image. Le calcul FIR Q15 reste le même binaire logique qu'en R1-DSP-A, mais le passage de D10 hôte à DWT physique ne qualifie pas toutes les erreurs physiques du capteur.

**Préservation :** TR2-001 n'a pas été flashé/debuggé/reset/commandé selon rapport, mais est signalé temporairement privé d'alimentation pendant essais et **son état actuel n'a pas été relu**. Le service PC est laissé en place, ses pertes sont explicites ; ne pas transformer cette absence de mesure en confirmation positive que TR2-001 est en RUNNING ou que son image/FRAM courantes sont inchangées.

## Décision A-01 et dépendances restantes

**Accepté :** bench physique isolé R1-ARM sur TR2-002 dans la fenêtre de **10 secondes**, avec FIR Q15 /10, FIFO SPI3 unique, horodatage causal contrôlé, DWT apparié et STOP ; **aucune anomalie observée dans ce domaine**. Les limites `work/service/gap` restent des références de banc, non des garanties industrielles universelles.

**Non accepté / à exécuter séparément :** FRAM SPI1, RTU LPUART1, composant SD SPI2, écriture 16 000 B/s, fichier archive, données durablement lisibles sur PC, endurance 1h/8h, alimentation TR2-001 après essais, métrique dynamique AC1 Windows et qualité sur machine. **Aucun GO industriel, aucune prétention d'enregistrement continu de 10 minutes.**

**Prochaine mission recommandée :** qualificatif SPI2 microSD isolé sur TR2-002, sans accaparer SPI3 et sans changer TR2-001. Préflight physique utilisateur et identification du média avant écriture ; lecture seule d'abord, puis écriture bornée/lecture inverse/intégrité et busy tail. Une mission ultérieure seulement rassemblera FIFO+FIR+SPI2 et les autres services, en mesurant les gaps FIFO.

**Statut de fiche :** A-01 positif **physique borné 10s**, mais pas une qualification `Mnnn` ni un tag. Aucun fichier de gouvernance normative gelée n'a été modifié par ce verdict.
