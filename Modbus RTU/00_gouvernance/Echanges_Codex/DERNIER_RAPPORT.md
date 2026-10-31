---
mission_id: TR2-20261031-R5-WM-CONFORT-INDICATIF-OFFLINE
mission_revision: R1-20261031-PREFERENCE-BV
status: IN_PROGRESS
base_ref: main
base_sha: 4192f4cbe0436a30cfe57a0033bbd869612931d5
initial_head: 62e92d9e8b9e3cd71e68f4297549d33ca239f800
result_sha: e7da10987dac8c5f8f8bc75f90ae6d5111eee938
created_at_utc: 2026-10-31T14:38:47.293484+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# TR2-Wm : calcul local livré, comparabilité BV non établie

Le traitement des six heures et les tranches logicielles sont terminés ; le statut
provisoire IN_PROGRESS porte sur la publication/relecture finale. **TR2_WM_A_MAX**
est calculé sur XYZ nominaux : RMS Wm de chaque axe, puis maximum sur la même
fenêtre, en **mm/s²**. Ce n'est ni une norme vectorielle ni un pic. Instrument
non étalonné, campagne **NON_CLASSÉE**, **COMPARABILITE_BV_NON_ETABLIE**. Aucun
seuil/ratio/classe BV ni substitution GOST, aucune conformité ou santé déduite.

## Résultats mesurés sur codes archivés, gain nominal

| Fenêtres FULL | P50 a_max mm/s² | P95 | Maximum |
|---|---:|---:|---:|
| MCU60s,365 | 43,728 | 74,344 | 99,102 |
| MCU120s,182 | 43,893 | 74,218 | 87,776 |
| QPC estimé60s,365 | 43,436 | 73,867 | 98,492 |
| QPC estimé120s,182 | 43,600 | 73,744 | 87,233 |

Période entière valide MCU :58 525 510XYZ, RMS XYZ23,379/18,643/43,874mm/s²,
axe gagnantZ/max43,874mm/s². Elle inclut la dernière fenêtre partielle et ne
moyenne pas les RMS. Premier/dernier segments incomplets restent PARTIAL, pas0.
3306résultats au total (deux horloges×trois variantes×551fenêtres60/120s).
L'axe gagnant et les valeurs non arrondies sont conservés pour chaque fenêtre.
Secondaire dépendant : équivalent v_w=0,028a_w, soit1,22847mm/s sur la période.
**Pas une deuxième mesure indépendante**, ni la vitesse non pondérée antérieure.

182paires120s par méthode reconstruites par somme d'énergie des deux60s : erreur
max4,44e−16, rang max60/RMS120 Spearman0,98296 pour MCU principal. Rapport max60/
RMS120 : médiane1,01687/P951,09822/max1,30128. La variation de durée lisse les
niveaux variables ; elle ne traduit pas une erreur de pondération. Les distributions
FULL60/120 ont des populations de bord légèrement différentes, séparées des
comparaisons alignées sur le même contenu.

Cadence QPC alternative : écart principal60s −0,7224% à−0,4022% par rapport àMCU.
Variante diagnostique du coin bas×0,8 :+0,004% à+3,748% ; ×1,2 :−1,515% à−0,004%.
Ce ne sont pas d'autres KPI principaux ni une qualification physique de1Hz.
Les filtres ont des raccordements lisses : diagnostic PSD Hann16s par blocs,
fractions pondérées médianes≈1,26% sous1Hz et2,49% au-dessus80Hz (fuite de fenêtre
incluse). Pas de découpage rectangulaire ou second passe-bande qui modifierait Wm.

**L'épisode extrême reste dans le principal.** Le diagnostic séparé exclut
17600–17630s des sommes d'énergie, sans changer source/filtre ni interpoler.
Variation du RMS global directionnel de période :−0,03608% ; part de l'énergieZ
sur ces30s0,2085%. Les fenêtres affectées du diagnostic gardent leur couverture
réduite ; aucune promotion FULL. Ces chiffres ne démontrent pas la cause du pic
source+7,676728g nominal, ni l'absence de défaut natif. Recommandation antérieure
±8g maintenue ; aucun registre ou acquisition modifié.

## Définition originale et validation numérique

Fonction analogique : Hh Butterworth2 à10^−0,1Hz × Hl Butterworth2 à100Hz ×
Ht ordre1 à1/(0,028×2π)Hz. Bilinéaire sans préwarping, SOS float64 : les limitations
sont incluses **une seule fois**. Pas de facteur appliqué au RMS large bande,
pas de réemploi de TEST_LP10. 1mg=9,80665mm/s². La vitesse équivalente est aussi
vérifiée par le transfert de pondération vitesse, distinct mathématiquement,
combiné à une intégration ; elle reste une grandeur dépendante.

Quatre oracles publics sélectionnés, aux centres réels tiers d'octave :
1/3,981…/10/79,4328…Hz avec gains arrondis0,833/0,818/0,494/0,0604. L'étiquette
nominale80Hz n'est pas la fréquence ponctuelle80Hz. Formule complète vérifiée
contre SOS complexe, phase et fréquence bilinéaire équivalente. Déformation
numérique maximale sur1–80 :0,4692% en module/0,00418rad en phase pourMCU.
Il ne s'agit pas de l'incertitude physique ni d'une tolérance ISO d'instrument.

**9 tests ciblés PASS** : réponse/phase, sinus0,5/0,8/1/2/4/10/25/80/100Hz,
multitons, AM/FM/bruit/chocs, offsets/inclinaison, DC/gaps/clips/vides,
chunking/causalité, unités/axe/vitesse, accumulation Parseval et SQLite.
Commande effectivement exécutée :

```sh
OPENBLAS_NUM_THREADS=1 /home/lolo/dev/msm/cache/r5-analysis-venv/bin/python -W error::ResourceWarning -m unittest discover -s experiments/r5_wm_offline -p 'test_*.py' -v
```

**72 contre-calculs réels PASS** : trois périodes faibles/médianes/fortes plus
période de l'épisode, chacune120s et ses deux60s, six variantes/horloges. Oracle
FFT fondé sur la fonction rationnelle indépendante, contexte±30s/padding60s et
Parseval. Écart RMS max9,024e−11 sous limite0,001 ; Parseval max2,586e−16 sous
limite1e−12. Aucun seuil de test affaibli.

Premier parcours arrivé au55econtrôle : RMS concordant mais somme float64
Parseval1,03895e−12>1e−12. Agrégats déjà sauvegardés : **pas de nouveau filtrage6h**.
Sommation long-double corrigée, test de dynamique ajouté, limite inchangée.
Reprise vérifie tous RMS/comptes/qualités contre moments sauvegardés, définitions,
SHA source et identité du core du checkpointe7da1098 ; seuls contrôles repris.
Échec, données intermédiaires et correctif sont conservés. Aucun ancien test ou
traitement PK/RMS/PREQUAL répété. Pas de tr2_validate/ARM/.NET : aucun impact.

Coût mesuré de reprise des contrôles12,304s/RSS333728KiB≈325,9MiB. Le coût total
initial n'a pas été enregistré après l'arrêt ; observation intermédiaire
RSS200208KiB, **pas un pic**. Pas de WCET/benchmark global inventé. Mémoire
structurellement bornée par blocs4096records, moments par seconde et quatre
contextes courts, aucune mise en RAM des58,55MXYZ ; NPZ local≈5,8MiB.
Le futur parcours sans arrêt consigne durée/pic global dans son manifeste.

Le code du filtrage évalué est le checkpoint e7da1098 (core inchangé) ; les SHA
runner corrigé/reprise et artefacts cache sont dans MANIFEST.json. Le reste des
modules assure contrôle, copie SQL, rendu et requêtes ; aucune application LIVE.

## Sources et comparaison BV, après validation du calcul

[Références/éditions effectivement accessibles](../../../experiments/r5_wm_offline/REFERENCES.md)
et [méthode/tolérances](../../../experiments/r5_wm_offline/METHOD.md).
Le registre officiel Rosstandart et le texte GOST2023 déclaréIDT à20283-5:2016
fournissent les définitions de grandeur/points. Relations historiques contrôlées
sur annexeA d'une adoption nationale **MOD** de2631-2:2003 et projetFDIS2026.
ISO confirme l'édition2026 ; son annexe normative finale n'est pas intégralement
visible surOBP. Le contenu public final décrit le même produit mais ses formules
ne sont pas intégralement récupérées en texte. **Concordance mathématique des
relations accessibles, pas preuve exhaustive d'identité des exigences finales
2003/2026/8041**. Algorithme local explicitement versionné, pas de coefficient
conjectural ni de norme achetée/PDF/table intégrale publiés. Téléchargement local
du PDF national expiré, consultation web utilisée sans annoncer un succès local.

**Étude LotB effectuée après les72contrôles validés**, trace dans
[BV_COMPARABILITY.json](preuves/TR2-20261031-R5-WM-CONFORT-INDICATIF-OFFLINE/BV_COMPARABILITY.json).
[BV NR467F juillet2026](https://rulesexplorer-docs.bureauveritas.com/documents/nr467/jul2026/467-NR_PartF_2026-07.pdf),
ch6 sec1§2.1.3(b), sec3§3.1/3.2.1/tableau4 : voie RMS globale pondérée1–80Hz
renvoyant àISO20283-5. Les tableaux5/6PK sont distincts et l'ancien blocage PK
n'est pas levé. Pont dimensionnel documenté avec0,028a_w, mais comparaison BV
**NON ÉTABLIE pour cette campagne** : identité exhaustive des références finales,
catégorie/occupation du local, axes/pose, conditions et couverture de mesure
non démontrés. Le mot «PC machine» ne désigne pas automatiquement une catégorie
BV ou un compartiment machines. Référence ISO20283-5 prévue pour voyages≥24h,
notre traversée≈8h sans garantie d'applicabilité. Aucune application réglementaire.

Tableau d'alignement point par point conservé ; catégorie NON_CLASSÉE,
seuil/ratio/classe nuls. Pas de ligne de seuil sur les figures ni de repère GOST
substitué. Une proportion de locaux n'est jamais une proportion des minutes.
L'absence d'étalonnage ne bloque pas le développement numérique local ; elle
limite son interprétation physique avec les autres réserves précises ci-dessus.

## Provenance, préservation et livraison

Archive locale :
`/home/lolo/dev/msm/reports/TR2-r3-6h-data-20261031/pc-recovery-after-powercut/E6N0005.TR2`.
Entier640MiB SHA91861cef14f824ae5b7422c55b092255654e2665062747205c7e034a0d1aeee5,
58 552 136XYZ/351 312 816B utiles,
SHApayload5077a1f6e290b563c254300fde4ab7fb1bb7409717e65831e73a20a277bb7e41.
SHA/CRC/index/session/qualité stricts avant/après reprise, queue préallouée exclue.
R3 CLOSED_WITH_RESERVATIONS maintenue : digest producteurRAM indépendant perdu ;
intégrité interne seulement. Gain nominal244µg/count/FSlogique8g, FS physique
continu non vérifié, pose/axes/cadence/bruit1Hz non qualifiés, UTC_UNKNOWN,
température absente. FIR107Q15 et son SHA/réponse déjà documentés inchangés,
pas de requalification physique ni de nouveau test FIR.

Copie unique cumulative **v5**,16tables historiques v4 intégralement conservées
(dont12anciennes v3),2204RMS/30804PK/8816PREQUAL intacts. Six méthodes et3306résultats
Wm ajoutés, une campagne, double import et audit idempotents aussi en octets,
integrity_check=ok/FKvide, cinq requêtes exécutées : comptes1/6/12/3306/1.
Les quatre anciens catalogues ont des SHA identiques avant/après ; preuve complète
[sqlite-proof.json](preuves/TR2-20261031-R5-WM-CONFORT-INDICATIF-OFFLINE/sqlite-proof.json).
DB locale `/home/lolo/dev/msm/reports/TR2-R5-WM-20261031/analysis-unified-v5.sqlite`,
57 704 448B, SHA48f02e019fc2e6d47a041690f93bd80a3f7241513aeec92c519aa9aa48f8c465.
Aucun XYZbrut ajouté en SQL, aucune DBLIVE ouverte ni base/TR2/NPZ/PDF publiée.

[Page/figures](preuves/TR2-20261031-R5-WM-CONFORT-INDICATIF-OFFLINE/index.html),
[note de résultats](preuves/TR2-20261031-R5-WM-CONFORT-INDICATIF-OFFLINE/NOTE_RESULTATS.md),
[preuves/CSV/chemins](preuves/TR2-20261031-R5-WM-CONFORT-INDICATIF-OFFLINE/README.md),
[CLI et limites pour futures archives](../../../experiments/r5_wm_offline/README.md).
Deux figures inspectées visuellement. Exports et identités non arrondis pour
reproduction ; valeurs invalides explicites. Champs BV du manifeste de calcul
portent l'état avant étude, sidecar final distinct et versionné, sans réécrire
les résultats de calcul. La CLI exige un SHA attendu et le profil strict connu.

Travail isolé MSM_vibe-R5-WM/codex/r5-wm-offline depuis62e92d9e. Dépôt principal
historique82169af, anciens worktrees et deux stashes6251b988/f3b829e6 préservés.
Mission/gouvernance/archives précédentes, code d'analyse antérieur, firmware,
Windows/service/FS/COM/SD/FRAM inchangés. Permissions exercées : logiciel, commit,
push normaux versmain seulement. Journal actualisé avec résultats/étude BV,
réserves et suite : revue indépendante ChatGPT, catégorie et contrôles instrumentaux
avant comparaison/documentation BV future. Aucune intervention physique requise ici.

G0 source/préservation satisfait ; G1 calcul local original et oracles satisfaits,
avec réserve explicite d'identité normative exhaustive ; G2 six heures/contrôles/
influence satisfaits ; G3 étude BV avec issue NON_ETABLIE satisfaite seloncontrat ;
G4 persistance/exports réalisés ; G5 publication/relecture en cours. Une clôture
DONE qualifiera seulement la livraison logicielle locale, aucune conformité ou
certification ni levée automatique des réserves physiques/historiques.
