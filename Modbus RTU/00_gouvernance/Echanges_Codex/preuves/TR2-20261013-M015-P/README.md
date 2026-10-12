# Preuves M015-P

Comparaison historique : offline-summary.json, historical-artifacts.json, I-source.c, transaction-comparison.csv. Préparation : commands.json, artifacts.json et logs host/control/causal ; image duplex compilée uniquement.

Contrôle physique unique : control-image.bin.hex et control-flash.bin.hex (byte-identiques O), identité/flash/readback/trigger/capture/final-detach avec scripts et journaux. control-capture.log conserve l'erreur de symbole GDB après sauvegarde des données. Les fichiers .original.hex préservent les octets originaux des journaux normalisés.

Données brutes : control-payload/traces/preclose/official-words.bin.hex ; décodages CSV, control-analysis.json et control-qualification.json. comparison.json établit la comparaison O/P ; user-intervention.json distingue témoignage et mesure. preservation.json relève les acquis Git historiques.

MANIFEST.sha256 couvre les fichiers de ce répertoire, à l'exclusion du manifeste lui-même. Aucun succès global de perte zéro ni cause précise n'est prétendu.
