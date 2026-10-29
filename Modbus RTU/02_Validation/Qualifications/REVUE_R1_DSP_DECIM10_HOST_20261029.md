# MSM TR2 — Revue indépendante R1-DSP-A /10 (hôte et cross-build, 29/10/2026)

**Responsable : ChatGPT ; statut : VALIDÉ LOGICIEL DANS LE PÉRIMÈTRE R1-DSP-A / NON QUALIFIÉ SUR CIBLE.** Il ne s'agit **pas** d'un jalon industriel `tr2/Mnnn` ; aucun tag de qualification matérielle n'est créé. Le programme STM32 n'a pas exécuté le DSP en temps réel dans cette tranche.

## Identités et lecture distante

- Dépôt `loloLR17/MSM_vibe`, branche `main`, HEAD lu au début de la revue : `76e30487184face381a047375adeee3015bef6ae`.
- Mission `TR2-20261029-R1-DSP-DECIM10-HOST`, `MISSION_EN_COURS.md` statut `READY`, même identifiant dans `DERNIER_RAPPORT.md` statut `DONE` ; ces états sont cohérents après publication du rapport. Mission autorisait commit/push sur `main`, **sans flash ni debug**.
- Commit source réellement revu : `0ad417d621c0023a2c326383040a5d4e73016329`, parent `545b96e1b8bec628219b0dd91c46fc51328d76cc`, 15 fichiers ajoutés/modifiés limités au FIR portable, CMake et tests. Commit des preuves : `45fb24d835aeac12d501b956bee8a75c94c4f1bd`, descendant direct ; HEAD documentaire final `76e30487184face381a047375adeee3015bef6ae`.
- Sources directement relues sur `main` : `Modbus RTU/05_Firmware/include/tr2/domain/acquisition/fir_decimator10.h` blob `fd8b129639ca914a9b77c2a273bb4b56df1bc446` ; `src/domain/acquisition/fir_decimator10.c` blob `83a6cf648c2994a2d9231e595393c45d7db73004` ; `src/domain/acquisition/fir_decimator10_coefficients.h` blob `3482b47db705df839638b0655b3ef02ab61eb7b2`. Code sans allocation dynamique, source/index/temps/segments validés, phase globale conservée, emission `ceil(N/10)` sans queue, arrondi nearest-even, clamp avec qualité séparée, fin de segment sur erreur. Absence de raccordement au callback FIFO réel **explicite**.
- Rapport [`DERNIER_RAPPORT.md`](../../00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md) et [preuves](../../00_gouvernance/Echanges_Codex/preuves/TR2-20261029-R1-DSP-DECIM10-HOST/README.md) lus ; `BUNDLE.json` contient 45 pièces et des empreintes de chacune, `remote-readback.json` rapporte 64 fichiers et 45 artefacts contrôlés par **Codex**. ChatGPT a vérifié directement la présence et le contenu des manifestes et sources GitHub ; **n'a pas refait une exécution de 149 tests ou décompressé intégralement chaque binaire de preuve**. Ne pas assimiler le contrôle documentaire indépendant à une seconde exécution matérielle.
- Compilateur rapporté : GCC ARM 14.2.1, `-mcpu=cortex-m33 -mthumb -O2`, ABI **soft-float** pour le profil isolé ; variante Q15 utilise `mla`, variante Q31 `smlal`. Baseline de linkage dédiée `text16/data0/bss2568`, Q15 `text2008/data0/bss3384` (soit +1 992B texte et +816B BSS, réserves pile/heap comprises dans le linkage). Ces `ELF` n'ont pas de table de vecteurs et ne sont **pas flashables**. Image canonique compilée inchangée en raison du code FIR non raccordé au runtime.
  
## Vérification numérique indépendante effectuée dans cette revue

Pour les **107 coefficients Q15 effectivement récupérés depuis GitHub** (et non pour un filtre float64 théorique), ChatGPT a calculé par une boucle trigonométrique indépendante du générateur SciPy et du script de tests Codex :

| Contrôle | Calcul indépendant | Conclusion |
|---|---:|---|
| Nombre de taps, symétrie | 107, parfaitement symétriques | Conforme |
| Somme Q15 | 32768 | Gain DC exactement 1 en arithmétique idéale |
| Somme des valeurs absolues | 47468 | Conforme |
| Borne `32768×L1` des sommes partielles | 1555431424, inférieur à 2147483647 | Un accumulateur int32 est suffisant pour le MAC direct déclaré |
| Max `abs(20log10|H|)` sur 0–700 Hz | **≈0,0027634 dB** (grille uniforme) | Très inférieur aux limites proposées |
| Pire stopband observé, à partir de `fout−700=1966,667 Hz` | **−67,4680 dB** vers 2327,88 Hz | Conforme sur grille |
| Majorant continu conservateur `peak + L·step/2 + 1e−12`, `L=(2π/fs)Σ|n−53||h[n]|` | **−66,5382 dB** | Conforme au seuil proposé de −60 dB, marge ≈6,54 dB pour les coefficients |
| Nombre et payload de 6s nominales | 160 000 entrées -> 16 000 XYZ -> 96 000 octets | Conforme |

Le calcul indépendant est un contrôle de la **réponse des coefficients**, pas un balayage complet de la logique C exécutée, ni une preuve des pertes, des erreurs analogiques du capteur ou des cycles processeur. La quantification d'entrée/sortie peut réduire la marge de rejet relative aux faibles niveaux ; Codex rapporte une borne tonale **−63,412 dB à A=12000 comptes** pour le modèle retenu. Aucune norme de rejet industriel universel sur amplitude quelconque n'en découle.

## Résultats de Codex acceptés dans leur portée

- Test C de lifecycle et non-régressions **149 PASS**, cross-build canonique ARM **PASS** selon logs publiés ; autres tests ciblés et ASan/UBSan PASS déclarés.
- **59 rejeux comparatifs C / SciPy** publiés, incluant trois empreintes externes float64 vérifiées/reproduites par Codex. Q15 exact à l'oracle quantifié de même coefficients et tolérance d'écart float64 définie, pas une identité bit à bit requise avec float64. Couverture D10-00 à D10-11 à l'échelle de la brique, D10-12/13 **non exécutés**.
- API de segmentation, index causal, clock rationnelle, WARMUP, source rails vs clipping FIR explicités, propriété d'état du caller. Valeurs int16 XYZ avec DC conservé **pour l'archive brute**, sans modifier le canal LIVE AC1 de mesures dynamiques. Ces deux sémantiques distinctes ne sont pas contradictoires.

**Verdict : R1-DSP-A clôturable logiciel** sur le code `0ad417d6`. **Pas de tag `tr2/Mnnn`**, faute de compilation exécutée de l'image avec liaison DSP au runtime et de tests CPU/FIFO physiques sur cible. Pas d'attestation que l'image actuelle du STM32 contient le DSP.

## Risques / portes suivantes

1. **R1-ARM :** sur TR2-002 identifié, relever les vrais cycles DWT sans/avec FIR, les distributions/maxima par rafale FIFO, work/service/gap, IRQ et pile high-water. Évaluer la marge Q15 ; Q31 reste un candidat comparatif si son coût et ses propriétés sont meilleurs.
2. Garder **un seul propriétaire FIFO**, réutiliser la reconstruction du temps, détecter les pertes et préserver les états AC1/B3/FRAM/RTU. La précision de l'horodatage transmise au FIR devra rester celle de la vraie autorité amont ; aucun temps inventé.
3. Ne pas confondre `f_source/10` avec une cadence murale absolument exacte ni le temps de groupe `53/f_source` avec le timestamp d'extrémité causal.
4. **Stockage SPI2 non traité** : producteur 16 000B/s nominal, DMA/file/backpressure/longues latences SD et récupération coupure à qualifier séparément. Aucune microSD, FRAM, Windows ni configuration ±8 g n'a été modifiée dans R1-DSP-A.
5. AC1 précédent demeure **`BLOCKED`** (signature Authenticode et gate Windows) ; ne pas réécrire son rapport historique comme terminé.

**Traçabilité de décision :** contrôle distant ChatGPT des 107 coefficients et commits, statut `VALIDÉ LOGICIEL / NON QUALIFIÉ SUR CIBLE`, le 29/10/2026. Le commit de publication documentaire de cette fiche ne change pas le SHA de code validé.
