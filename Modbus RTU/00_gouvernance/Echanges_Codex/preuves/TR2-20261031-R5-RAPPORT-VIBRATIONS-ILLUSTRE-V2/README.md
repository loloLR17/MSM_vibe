# Rapport illustré V2 — preuves éditoriales

Livrable : [PDF de dix pages](../../../../02_Validation/Rapports/MSM_TR2_Rapport_capacites_vibratoires_31-10-2026.pdf).

- `build.json` : source PDF réelle sur a9077dcf, empreintes et composition. Les xref y sont ceux avant optimisation ; ceux du PDF final sont dans verification.json.
- `verification.json` : lecture indépendante du PDF produit, dix rendus, pagination/bornes/texte, trois extractions de pixels RGB identiques aux PNG sources. Résolution effective 177–180 dpi, dimensions natives conservées, aucune compression destructive.
- `source-preservation.json` : comparaison intégrale de tous les fichiers des deux dossiers de preuves R5/Wm à la référence initiale, sans modification.
- `remote-readback.json` : ajouté après publication et relecture intégrale depuis la référence GitHub récupérée.

Reproduction depuis la racine du dépôt avec Python 3 et PyMuPDF 1.26.7 existant (fallback local `/home/lolo/dev/msm/cache/r5-bv-pk-pdf-deps`, aucune installation globale) :

```bash
python3 'Modbus RTU/02_Validation/Rapports/outils/illustrate_report_v2.py' /tmp/r5-v2-build-neuf
python3 'Modbus RTU/02_Validation/Rapports/outils/verify_report_v2.py' /tmp/r5-v2-verification-neuve
```

Le premier outil remplace seulement le PDF cible à partir de la référence initiale fixe ; le second vérifie le document courant et crée un dossier neuf. Aucun accès signal, base SQLite ou matériel. Les chiffres affichés proviennent des synthèses déjà validées, uniquement arrondis. Les courbes vectorielles temporelles originales restent regroupées par énergie sur cinq minutes ; les statistiques sont celles des fenêtres complètes 60/120 s.

Les dix rendus ont été réellement produits et inspectés visuellement (couverture, confort, RMS, trois figures et quatre annexes/sources). Ils sont conservés localement dans `/home/lolo/dev/msm/reports/TR2-R5-rapport-V2-20261031/verification-v2/`, leurs empreintes RGB sont publiées dans verification.json. Pas de chevauchement ni page vide observé. Les pixels extraits des trois images finales sont identiques aux originaux ; la présentation n'invente aucune figure.

Le PDF précédent de huit pages reste dans Git à a9077dcf et dans `original-main.pdf` du dossier local de preuves. La catégorie d'occupation provient du présent contrat, sans réécrire le manifeste comparatif Wm antérieur. Repères BV contrôlés dans NR467 Part F juillet 2026 ch.6 §3 tableau4, page imprimée245/PDF247 ; aucune classification de la pièce.
