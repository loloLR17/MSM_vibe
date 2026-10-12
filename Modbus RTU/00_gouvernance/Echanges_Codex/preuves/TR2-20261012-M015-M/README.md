# Preuves M015-M

Deux acquisitions uniques et distinctes : fifo-O0 (refus connu reproduit),
fifo-O2 (deux transferts complets, puis capacité1024 et zéroXYZ). Cleanup-O2
est une séquence de mise au repos seule, aucune acquisition ni succèsFIFO.
Sources évaluées a7a2ebc ; image cleanup ajoutée ensuite, sourcehash dansrapport.

*.bin.hex sont les octets exacts, à reconstruire avec bytes.fromhex ; les
*.flash.bin.hex sont les readbacks des seuls octets d'image programmés, pas
une sauvegardeFlash2MiB. Images disponibles en *-image.bin.hex, hashes dans
artifacts.json/cleanup-artifacts.json. Résultat4032, Trace40, Preclose76octets,
endianness little ; CSV conservés avec tous les champs. fifo-O2-words.csv
reste accepted=false malgré parité correcte (fenêtre incomplète/zéroXYZ).

Logs rendus LF et sans espacefinal ; les logsCLI nonUTF8 sont décodés cp1252,
avec *.original.hex préservant leurs octets. Originaux locaux dans dossier
TR2-mission015-M-20261012T204419Z. Manifestes SHA des livrables et gardes de
préservation disponibles. Aucun nouveau WHO/polling autonome, aucun TR2principal.
Les scripts documentent exactement les opérations consommées : ne pas les
relancer, chemins exclusifs et STOP sur anomalie nouvelle.
