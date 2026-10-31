---
mission_id: TR2-20261031-R5-ANALYSE-PC-MACHINE-6H
status: IN_PROGRESS
base_ref: main
base_sha: d053d70155aecfe5af9bf7059e3b0dd6ea24a7af
initial_head: ecbff71f735cc8e4c195d3882b887aaf3f6e5aec
result_sha: bb16202f1cfae5765961e64dc6125a9bae52ae5a
created_at_utc: 2026-10-31T09:14:59.556207+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# R5 — six heures analysées, catalogue SQLite rempli

**G1/G2/G3/G3b/G4 exécutés et satisfaits dans la portée exploratoire du contrat.**
Le calcul intégral, les contre-calculs réels et la persistance sont faits.
**IN_PROGRESS porte sur la publication/relecture de ce lot**, pas un blocage
matériel ni un arrêt au simple logiciel préparé. Aucun essai physique relancé.

[Paquet local navigable](preuves/TR2-20261031-R5-ANALYSE-PC-MACHINE-6H/index.html),
[synthèse métier](preuves/TR2-20261031-R5-ANALYSE-PC-MACHINE-6H/SYNTHESE_POC.md),
[figures et CSV](preuves/TR2-20261031-R5-ANALYSE-PC-MACHINE-6H/README.md),
[méthode reproductible](../../../experiments/r5_campaign_analysis/README.md),
[étude vitesse/ISO/BV](../../../experiments/r5_campaign_analysis/VELOCITY_FEASIBILITY.md).
Sur GitHub, les PNG/Markdown sont lisibles directement ; télécharger le paquet
pour ouvrir index.html hors ligne. Le HTML brut GitHub n'est pas un site déployé.

## Résultats calculés sur E6N0005, à sensibilité nominale

| Fenêtre relative MCU | RMS global médian | P95 | Maximum RMS global |
|---|---:|---:|---:|
|1s|13.032mg|22.912mg|58.398mg|
|10s|13.318mg|24.500mg|44.580mg|
|60s|13.361mg|25.643mg|34.405mg|

Indicateur filtré Butterworth2–25Hz ; crête vectorielle simultanée maximale
**145.636mg**, distincte des crêtes d'axes. RMSglobal=√moyenne(x²+y²+z²).
Le brut est livré séparément, moyenneXYZ≈(51.33;−88.07;1018.62)mg avec DC.
Aucun résultat n'est appelé vitesse pondérée ni indice de confort humain.

- **Couverture :**58552136XYZ examinés,11warmups FIR exclus du brut statistique ;
 58538817sorties valides pour la bande, soit99.97725%, après13308sorties
 supplémentaires de stabilisation5s. Aucun saut horodaté>10périodes ; intervalle
 maximal377µs. Les absences restentNaN/NULL et les couvertures sont exportées.
- Fenêtres nominales entières/coverage99–101% :21994sur22000(1s),2198sur2200(10s),
 365sur367(60s). `VALID_ESTIMATED` n'affirme pas chaque point présent ; la
 fenêtre1s àt=5s conserve environ995ms après stabilisation. Première minute et
 dernière minute partielle grisées ; les quantiles ci-dessus excluent les
 fenêtres hors ce critère. Pas de lacune remplacée par zéro.
- **Épisodes relatifs :**P90 des365minutes=15.34524mg ;5épisodes/37minutes,
 10.14% des minutes retenues. Plus long17580–19260sMCU(28minutes), seuil interne
 à cette campagne, sans signification de gêne normative.
- **Fréquences :**spectrogramme complet3572trames, première invalide/grisée ;
 Hann2048après/16, pas6.156s/fenêtre12.312s, résolution0.08122Hz estimée.
 PSDglobale=sommePSDX/Y/Z, mg²/Hz ; couleur10log10(PSD/[1mg²/Hz]).
 Trois minutes choisies selon RMS faible19500–19560s, médian12420–12480s,
 fort18540–18600s ; PSDWelch Hann32768/50% sur signal causal, sans detrend.
 Bandes candidates centrées19.574/20.630/24.366Hz, ±0.30Hz ; faibles contrastes
 et sauts masqués. CSV et quatrième figure distinguent fréquence et puissance.
 Aucun équipement, arrivée, manœuvre ou régime n'est attribué sans témoin externe.

## Provenance, temps et limites scientifiques

Source immuable :
`/home/lolo/dev/msm/reports/TR2-r3-6h-data-20261031/pc-recovery-after-powercut/E6N0005.TR2`.
Taille671088640B, dont351312816B utiles ;750669records/58552136XYZ.
SHA entier **91861cef14f824ae5b7422c55b092255654e2665062747205c7e034a0d1aeee5**
confirmé avant et après traitement ; SHA utile
**5077a1f6e290b563c254300fde4ab7fb1bb7409717e65831e73a20a277bb7e41**.
Le lecteur R3 versionné vérifie strictement COMPLETE/CRC/indices/session/temps/
compteurs/footer, rails0/clips0, qualitéOR1(warmup). Queue préallouée exclue.

Session8444550407367521 ; UID001b0046/38425018/20333157 ; firmware capturé
c3cd40ce9e96cef2241fb5bc6e8c83acd2a3824e. Sensibilité244µg/count nominale,
±8g logique/FIR107Q15DEC10/DC conservée. AxesXYZ capteur, orientation navire
inconnue ; point annoncé plancherPCmachine/double-face mince, montage non qualifié.

**R3 reste CLOSED_WITH_RESERVATIONS** : le digest utile correspond au footer,
mais le digest indépendantRAM a été perdu à la coupure. Le libellé historique du
lecteur «producer digest matched» n'est pas une récupération de cette preuveRAM.
Aucun re-classement industriel de la chaîne producteur→microSD ni qualification8h/V1.

**UTC_UNK2026-10-31T09:14:59.556207+00:00N.** DSP sur grille régulière estimée2661.4620215Hz par endpoints
MCU, distincte des2660.9794952Hz initiaux du header. Durée21999.988926sMCU ;
écart maximal grille/interpolation archivée0.605751sMCU. Le témoin QPC original
six-long.json(SHAee4c3a65…1479eb, identique au bundle R3 publié) prouve un
intervalle actif21601.0193586s et estime2679.5084–2679.5240Hz via compteurs/10
et phase±1sortie. Différence de bases≈0.678%, sans étalonnageSI ni correction
physique arbitraire. Ces bornes sont conditionnelles à cette convention de
snapshots ; finesseFFT≠exactitudeHz. STARTWindows/dateFAT ne datent pas les sorties.

Filtre Butterworth prototypeordre4/total8 SOS float64 causal, état conservé entre
lots, initialisation constante et5s invalides à chaque reprise. Gain−3.0103dB
aux bornes2/25Hz,−0.462dB à20Hz : **indicateur à réponse documentée, pas bande
rectangulaire idéale**. Retard causal non compensé. Avant décimation spectrale,
réjection finie−63.31dB à141.34Hz se repliant à25Hz ; alias nul non garanti,
aucune bande2–700Hz qualifiée. Incertitude d'amplitude/pose/bruit non établie.

## SQLite réellement rempli, isolé de la supervision

Base locale finale :
`/home/lolo/dev/msm/reports/TR2-R5-analysis-6h-20261031/r5-catalog-v1.sqlite`
(19292160B), SHA256 **e436de69e55e2126c029acc5afea9cb058b0a6fc6ce857e41bc70a26693646c5**.
Analyse **d7d3284803baec695110313113f7267c374f5d1d3c3a0583d469eb1d1fd8e044**.

Schema1/application_id propre :1campagne,1analyse,**49134métriques**,
7référencesSHA aux fichiers externes ; aucun XYZ brut/préallocation en SQL.
Pour chaque indicateur brut/bande :22000fenêtres1s,2200fenêtres10s,367fenêtres60s.
Clés/index/transactions/FULL/FK, versions/paramètres/bande/unité/qualité/provenance
présents ; identité dépend de la méthode, échelle/cadence et empreinte du code.

Deux imports réels sur la base finale : mêmes comptes/identité, second idempotent.
`integrity_check=ok`, `foreign_key_check=[]`. Quatre requêtes réellement exécutées :
liste1campagne ;367minutes (bords signalés) ; pire10s à18610sMCU,44.57988mg ;
60lignesXYZ à600–660s. Schéma et requêtes SQL versionnés, journal d'erreurs
absent/corrompu testé. Base finale sansperturbation de la DB Windows existante.

Inspection des composants .NET existants : persistenceSQLite schema7/B3 LIVE/
receivedUTC/mg entiers et TR2.Campaigns squelette ne représentent pas l'archive
massive. Reprise des conventions SQLite plutôt que migration risquée ou recodage
.NET. Frontière future : adaptateur lecture parcampaign/analysis_id/temps relatif.

Correction de revue : la signature des résultats est fondée sur les CSV
**décompressés** pour que l'horodatage/nom du conteneur gzip n'invente pas une
modification numérique. Test supplémentaire de réimport depuis un autre
conteneur PASS. Le catalogue initial est conservé séparément ; aucun calcul
scientifique ni testphysique n'a été répété pour cette correction de persistance.

## Validations, erreurs, coûts et fichiers livrés

**14tests ciblés PASS** : DC/gravité, sin2/10/25/80Hz, impulsion, multiton,
frontières de lots/fenêtres, gaps/NaN, RMS vectoriel/pondération, PSD/Parseval,
anti-alias fini, intégration synthétique1/2/5/10/20/40/80Hz+offsets,
SQLite/import/requêtes/idempotence/version et refusDB étrangère/export altéré.
Tolérances fixées avant passage ; aucun test affaibli ou supprimé.

**Trois contrôles réels indépendants PASS** : convolutionFFT avec transfert
calculé algébriquement face aux SOS causaux (tolérance relative0.1%, observé
≤1.087e-12), Parseval face au RMS brut (tolérance1e-8mg, observé≤5.457e-12mg).
Ce n'est pas deux appels au même filtre ; aucune calibration matérielle déduite.

Calcul intégral24.52s dansl'outil, picRSS397320KiB≈388MiB, lots4096records ;
cache décimé local43.91MB, pas58552136objets Python. Trois figures initiales puis
quatre figures lisibles rendues depuis les exports, **sans refaire le filtrage**.
TestsPython proportionnés : firmware/.NET/interfaces stables inchangés, donc pas
de build ni revalidation matérielle pertinente pour cette mission PC.

Erreurs conservées : ensurepip absent/venv initial incomplet puis bootstrappip
privé parurllib(curl indisponible), première invocation avantnumpy installée ;
prototypeSQL13colonnes/14placeholders corrigé. Logs d'échecs et succès publiés.
Environnement privé cache/r5-analysis-venv, dépendances gelées ; aucune installation
globale, modification de sécurité Windows ou de gouvernance. Source originale,
anciennes archives/tests/supervision préservés.

Étudevitesse : synthèses exactes en binsFFT, erreur spectrale max8.89e-16,
trapèzes indépendants0.12553% à80Hz, dans0.2% fixé. **Wm non implémenté** ;
aucune vitesse réelle/gradeBV/ISO annoncé. Après livraison des graphes réels,
plus petite suite proposée : verrouiller éditions/fonctionWm, essai court avec
fixation/orientation documentées et référence étalonnée incluant1Hz et cadence,
puis qualification intégration/pondération. Aucun budget/ROI ni gain de confort
inventé ; tramePOC humaine préservée et synthèse distincte remplie factuellement.

Les preuves compactes publiées représentent≈7.5MB ; pas de .TR2/DB/cache43MB/PSD11MB
en Git. Chemins et SHA des artefacts réels dans LOCAL_ARTIFACTS.json ; manifest,
CSV1/10/60, PSD3plages, raies, épisodes,4PNG, HTMLhorsligne, schéma/tests/code.

## Git, permissions et clôture

PremierHEAD observé ecbff71f ; addendum SQLite/tramePOC repris avant tout commit
propre surd5c9bb7a dans worktreeisoléMSM_vibe-R5-analysis-current. Préparation
antérieure intégralement conservée dansMSM_vibe-R5-analysis-6h. Code numérique
évalué **bb16202f1cfae5765961e64dc6125a9bae52ae5a**, publié/12fichiers relus
intégralement byte-exact depuisorigin/main (checkpoint-remote.json).
Après ce SHA, changements fonctionnels limités à la persistance gzip ;
présentation/graphes et tests complémentaires, sans refaire les résultats acquis.

Dépôt historique main82169af7, worktree récupérationd6b4e179 et deux stashes
6251b988e8b20f97997a8f98fd26d4d35a45a1b2 /
f3b829e6188f491920edf74a5f9ed0f559d08b87 préservés. Pas de reset/clean/merge/
rebase/forcepush, aucune commandeSTM32/flash/GDB/SD/COM ni serviceWindows.
Dernier état physique déclaréTR2-002hors tension/SDauPC, non revérifié physiquement.
Permissions exercées : développementPC/commit/pushnormal surmain seulement.

JournalETAT_COURANT_TR2 actualisé : analyse réelle acquise, catalogue, limites,
suite de qualification ; décisions normatives/architecture inchangées.
Prochaine action immédiate : commit de ce lot, pushnormalmain, relecture complète
GitHub puis rapportDONE siG5 satisfait. Revue indépendanteChatGPT ensuite.
