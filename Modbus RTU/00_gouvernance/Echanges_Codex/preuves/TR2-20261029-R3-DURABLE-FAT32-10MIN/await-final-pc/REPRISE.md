# Reprise — uniquement extraction PC et publication finale

Mission active TR2-20261029-R3-DURABLE-FAT32-10MIN, non clôturée.

- Worktree /home/lolo/dev/msm/projects/MSM_vibe-R3-durable ; main historique82169af préservé.
- Firmware physique6fdae042333f671629d2c4626d7120b0bd848ae3 ; BIN27f5d59fc58803d6a7fd66714014ca7d24da60023736386b3375b9272fbf9ef6.
- Cible exclusiveTR2-002/SN0032001F3235510C37333439/UID001B0046-38425018-20333157.
- 2/60/600s nouveaux R2N acquis :600,000520s/1585692XYZ/9514152B, zéroOVR/SD/filefaute, filepeak5632B/64KiB. STOP151us/drain11733us, footerack/SDquiescente/CShaute/GDBdétaché.
- AncienessaiCMD24 arrêté265s/filepleine conservé, RUN0003/4 complets vérifiésPC, RUN0005 incomplet690846XYZ/4145076B vérifiéCRC/session/index, pasSHAcomparé àsourceentière.
- Dernièreconfirmationutilisateur avant600s : carte réinsérée/ST-LINK connecté/USBnatifoff. Retraitfinal demandé aprèsquiescence ; **attendre sa confirmation** avant lecturePC. Aucun nouveauflash/debug/reset/capture nécessaire.
- Preuves P=/home/lolo/dev/msm/reports/TR2-r3-durable-fat32-10min ; scripts et JSON conservés. Aucuneacquisition en cours : group-file-600s/result.json stage4/error0, helperterminé.

Àréception « carte dans le lecteurPC/TR2-002 off » :

1. Vérifier Git réel/fetch et rapport courant sans écrasement ; ne pas relancer les captures.
2. `python3 /home/lolo/dev/msm/reports/TR2-r3-durable-fat32-10min/extract_group_pc.py` : vrais GetVolume/GetDisk F:, copie3R2N, parserPython, SHA tousoctets producteur avantTX,128préfixes capturés etcoefficientSHA ; anciens3RUN re-hachés inchangés. Sortie pc-group-final/INTEGRITY.json. Si échec, préserver le dossier partiel ; analyser et utiliser un nouveau dossier de retry sans suppression.
3. Seulement si cette vérification réussit : `python3 /home/lolo/dev/msm/reports/TR2-r3-durable-fat32-10min/make_final_report.py` produit rapportprovisoireIN_PROGRESS etmanifestefinal-PC (les preuves de la lecturePC sont requises par assertions), actualisejournal.
4. Relire diff/rapport, commit/push normalexplicitmain, fetch ; `python3 /home/lolo/dev/msm/reports/TR2-r3-durable-fat32-10min/verify_published.py provisional-final` relit touslesobjets GitHub etdécompresse BUNDLE +vérifieprovenancevendor.
5. Consigner le SHA relu, passer rapportDONE dansportée10minPC seulement, archiverMISSION/RAPPORT àl’identique dans un dossier neuf (pasd’écrasement), journalactualisé ; pushnormal/fetch/relecturefinale. Fournir SHA distant etgitstatus.

Ne pas toucherTR2-001/FRAM/COM4/serviceWindows. PasGO1h/8h/powerloss/2000Hz normatif/SI. DCpréservée, interpolation temporelle explicite/UTCinconnue. Imagegardée enFlash, pasrestaurationgratuite. CarteFAT32test, aucunformat oueffacement supplémentaire. Deuxstashes historiquespréservés.
