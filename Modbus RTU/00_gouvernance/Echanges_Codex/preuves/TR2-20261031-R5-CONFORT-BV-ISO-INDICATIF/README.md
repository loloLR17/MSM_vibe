# PC machine — résultats préliminaires1–80Hz, P0 bloquée

Ouvrir [index.html](index.html) localement, sans réseau. [Synthèse POC](SYNTHESE_POC.md).
[Graphique réel](unweighted_trends.png), [toutes fenêtres/axes/qualité](metrics.csv),
[statistiques](summary.json), [définitions versionnées](definitions.json),
[effet d'horloge](clock_sensitivity.csv), [contre-calculs FFT](real_crosschecks.json),
[réponse FIR](fir_response.csv), [SQLite/passerelle](sqlite-proof.json).

Statut scientifique : **EXPLORATOIRE_NON_NORMATIF**, numériquement testé,
physiquement non étalonné. Wm, équivalentISO, classe ou repèreBV absents.
Le maximum des RMSXYZ du capteur ne constitue pas une direction navire validée.
La vitesse est une intégration physique approchée dans une bandeButterworth,
pas nécessairement la grandeur équivalente d'un référentiel. Les chiffres ne
permettent pas un jugement «acceptable», une exposition individuelle ou une
attribution de source machine.

[Méthode PC/reproduction](../../../../../experiments/r5_comfort_analysis/README.md),
[ficheP0 et sources manquantes](../../../../../experiments/r5_comfort_analysis/P0_METHOD.md).
L'utilisateur confirme l'absence d'accès autorisé. Le contrat autorise dans ce
cas les sorties non pondérées et demande un retourBLOCKED après publication.

HypothèseMCU2661.462Hz / hypothèseQPC2679.516Hz : **mêmes fenêtres60/120sMCU**,
fréquences/integration recalculées, sans convertir les fenêtres en secondesSI.
Wm non reconstruite depuis des extraits non vérifiés. Aucun coefficient constant
appliqué aux13.36mg de l'ancienne analyse. Anciens2–25Hz/PSD/raies conservés dans
le [paquet précédent](../TR2-20261031-R5-ANALYSE-PC-MACHINE-6H/index.html).
Pour disposer des deux familles horsligne, conserver leurs deux dossiers voisins.

Filtres1–80Hz : prototypeButterworthordre4/total8 causal,−3.0103dB auxbornes,
rollofffini ; étatpersistant entrelots et10s de stabilisation aprèsreprise,
11warmupsFIR exclus. FULL exige autantdesortiesvalides qu'entréesobservées et
durée nominale entière. InvalidesNaN/NULL, jamais zéro. La dernière fraction de
fenêtre n'est pas extrapolée ; bords exclus des quantiles. Échelle, axes et pose
non étalonnés ; chronologieUTC_UNKNOWN ; digestRAM indépendant toujoursperdu.

La vitesse stable est obtenue parB(s)/s, zéroorigineannuléavantbilinéaire ;
sonrapportàl'intégrateuridéal est(πf/fs)/tan(πf/fs), jusqu'àenviron0.30% debiais
à80Hz. Surles3plagesréelles, contrôleFFTidéal contretrapèzes : écart≤0.0582%,
souslimite1%fixée. ContrôleFFTalgébrique contrechaînetemporelle :≤3.893e-11relatif,
souslimite1e-6fixée. Il ne s'agit pas de tests de Wm.

Réponse linéaire du FIR107Q15 vérifiée parSHA égaleàl'archive : gainDC1,
atténuation calculée≈0.000351dBà80Hz, délai≈1.991msMCU,−77.73dB au point
fs_sortie−80Hz. Cela ne qualifie pas toutl'anti-aliasing, le MEMS, sa réponseà1Hz,
son bruit ni le montage. Arrondi sortiebornéà0.122mgnominal/axe, sanshypothèse
que son erreur soit blanche ou indépendante. Voirlescoefficients etlimites du capteur.

**11tests ciblésPASS**, sansrelancerles14anciens tests inchangés. LectureCRC/SHA
et calcul complet surles58552136XYZ en43.18s/picRSS274232KiB≈267.8MiB ;
12contre-calculs (3segments×2bases×2unités). Aucune opérationSTM32/flash/debug/
SD/COM ou modification d'environnement global/sécuritéWindows/baseLIVE.

Catalogue secondaire local626688B/schema2 :1campagne/4définitions/4analyses/
2204fenêtres/24références,2importsidem/quatrerequêtes/intégritéPASS. Anciennebasev1
49134métriques SHAe436de…93646c5 inchangé avant/aprèspasserelleread-only.
AucunXYZbrutenSQL, aucune DB niarchive640MiB envoyée àGitHub. Chemins/SHA exacts
locaux dans LOCAL_ARTIFACTS.json et sqlite-proof.json ; code enexperiments/r5_comfort_analysis.
