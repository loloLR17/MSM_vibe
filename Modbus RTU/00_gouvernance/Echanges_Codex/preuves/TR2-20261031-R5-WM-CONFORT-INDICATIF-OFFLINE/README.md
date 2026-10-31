# TR2-Wm — preuve de livraison PC uniquement

[Résultats expliqués](NOTE_RESULTATS.md) · [Page et figures](index.html) ·
[Code/CLI](../../../../../experiments/r5_wm_offline/README.md) ·
[Méthode/tolérances](../../../../../experiments/r5_wm_offline/METHOD.md) ·
[Sources/éditions effectivement consultées](../../../../../experiments/r5_wm_offline/REFERENCES.md).

- [Fenêtres/axes/max non arrondis](metrics.csv), [définitions/coefficients/hash](definitions.json), [résumé](summary.json), [période entière](whole_period.json).
- [Paires alignées60/120](aligned_pairs.csv), [stabilité de durée](duration_sensitivity.json), [sensibilités](sensitivity.csv), [réponse module/phase](frequency_response.csv), [déformation numérique](warping.json).
- [Influence séparée de l'épisode](event_influence.json), [diagnostic PSD hors bande](out_of_band_diagnostic.json).
- [Neuf tests PASS](tests-final.log), [72 contre-calculs réels PASS](real_controls.json), [première limite Parseval dépassée](failed_controls.json), [trace initiale](full-v1.log), [reprise sans refiltrage](controls-resumed-v1.log).
- [Étude BV réalisée après contrôles validés](BV_COMPARABILITY.json), [SQLite/préservation/idempotence](sqlite-proof.json), [manifest de reprise](MANIFEST.json), [chemins locaux](LOCAL_ARTIFACTS.json).

Le principal TR2_WM_A_MAX est le max des trois RMS, jamais la norme vectorielle
technique secondaire présente dans le CSV pour audit. Un seul indicateur principal ;
LOW08/LOW12 sont diagnostiques, `0.028*a_w` est un équivalent dépendant. Une
fenêtre partielle n'est pas promue FULL, l'épisode extrême reste dans le principal.
Pas de seuil automatique, de ratio àBV ni de catégorie déduite du mot PCmachine.

MANIFEST/definitions portent l'état **au moment du calcul**, avant étude BV
(`NOT_YET_STUDIED`/`PENDING_LOT_B`). Le résultat ultérieur est séparé et versionné
dans BV_COMPARABILITY.json : COMPARABILITE_BV_NON_ETABLIE. Les métadonnées de
seuil ne modifient pas la valeur physique calculée. Aucun seuil GOST adopté.
Les preuves de fonctions/points accessibles ne démontrent pas l'identité
exhaustive de toutes les dispositions ISO finales ; algorithme local indicatif.

Le premier parcours avait sauvegardé tous les agrégats avant les contrôles.
Une somme float64 a produit une erreur Parseval1,03895e−12 malgré un accord RMS
3,9e−11 : accumulation long-double corrigée, tolérance1e−12 inchangée. Reprise
vérifie SHA source, lecteur strict, méthodes, toutes les lignes RMS/comptes/qualité
contre les moments sauvegardés et l'identité du core du checkpoint e7da1098.
**Aucun filtrage complet répété**, coût du parcours initial non enregistré
complètement après l'arrêt ; ne pas transformer l'échantillon RSS observé en pic.
Contrôles repris12,304s/RSS333728KiB, ce n'est pas le coût total de la mission.

Une copie dérivée v5 conserve toutes les16tables v4 (dont les12anciennes v3),
2204 anciens RMS/30804PK/8816tests de préqualification, et ajoute3306résultats/
six méthodes Wm. Double import et audit idempotents en octets, intégrité/FK/
requêtes réellement exécutées. V1/v2/v3/v4 byte-identiques avant/après ;
aucune DBLIVE ouverte. Pas de XYZbruts ajoutés en SQL ; TR2/DB/NPZ/PDF restent
locaux. Coefficients et quelques oracles factuels seulement publiés, aucune
transcription exhaustive de table ni de texte/figure protégés.

Source entier SHA91861c…1aeee5 et payload5077a1…bb7e41 stricts avant/après,
58552136XYZ, queue préallouée exclue. Réserve R3 sur digestRAM perdu maintenue ;
UTC_UNKNOWN, FScontinu/pose/axes/gain/bruit1Hz non qualifiés. FIR107Q15 inchangé,
réponse1–100 déjà démontrée réutilisée, pas de nouveau test historique. Ancien
PK BLOCKED conservé ; nouveau TR2-Wm numérique ne requalifie pas cette voie.
Aucun matériel/flash/debug/COM/SD/Windows, aucune capture/achat.
