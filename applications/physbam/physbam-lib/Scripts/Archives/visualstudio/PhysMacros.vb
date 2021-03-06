Imports EnvDTE
Imports System.Windows.Forms
Imports Microsoft.VisualStudio.VCProjectEngine


Public Module PhysMacros
    Sub RebuildProject()
        Dim exclude
        RebuildPhysBAM("PhysBAM", "\PhysBAM.vcproj")
        'RebuildPhysBAM("PhysBAM_OpenGL", "\PhysBAM_OpenGL.vcproj")
    End Sub

    Sub RebuildPhysBAM(ByVal project As String, ByVal root_postfix As String)
        Dim proj As Project = GetProject(project)
        Dim vcproj As VCProject = proj.Object
        Dim root As String = proj.FullName.Replace(root_postfix, "")
        Dim saveUI = DTE.SuppressUI
        DTE.SuppressUI = True
        ' get rid of other crap
        ClearProjectFiles(vcproj)
        ' traverse top level directories
        Dim subdir As String
        For Each subdir In System.IO.Directory.GetDirectories(root)
            If Not subdir.EndsWith("\CVS") And Not subdir.EndsWith("\Release") And Not subdir.EndsWith("\Debug") Then
                Dim folder As VCFilter = vcproj.AddFilter(Relative(root, subdir))
                TraverseAndFillFilters(root, subdir, folder)
            End If
        Next
        DTE.SuppressUI = saveUI
    End Sub

    Private Sub TraverseAndFillFilters(ByVal root As String, ByVal curr_root As String, ByVal filter As VCFilter)
        ' Go through subidrectories and recursively add
        Dim subdir As String
        For Each subdir In System.IO.Directory.GetDirectories(curr_root)
            If Not subdir.EndsWith("\Documentation") And Not subdir.EndsWith("\CVS") And Not subdir.EndsWith("\Release") And Not subdir.EndsWith("\Debug") Then
                Dim folder As VCFilter
                folder = filter.AddFilter(Relative(curr_root, subdir))
                TraverseAndFillFilters(root, subdir, folder)
            End If
        Next
        ' Add Files
        Dim file As String
        For Each file In System.IO.Directory.GetFiles(curr_root)
            If file.EndsWith(".c") Or file.EndsWith(".cpp") Or file.EndsWith(".h") Then
                filter.AddFile(file)
            End If
        Next
    End Sub

    Private Function Relative(ByVal root As String, ByVal directory As String)
        Relative = directory.Replace(root + "\", "")
    End Function

    Private Function GetProject(ByVal projname As String) As Project
        Dim proj As Project
        For Each proj In DTE.Solution.Projects
            If proj.Name = "PhysBAM" Then
                GetProject = proj
            End If
        Next
    End Function

    Private Sub ClearProjectFiles(ByVal proj As VCProject)
        Dim filter As VCFilter
        ' seems to be bugs in removefilter... so iterate until fixed point...
        Dim count = 1
        While count > 0
            count = 0
            For Each filter In proj.Filters
                proj.RemoveFilter(filter)
                count = count + 1
            Next
        End While
    End Sub
    Sub MakeOpposite()
        Dim oldname As String = DTE.ActiveDocument.Name
        Dim myname = DTE.ActiveDocument.Name.Replace(".h", ".cpp")
        Dim myfullname = DTE.ActiveDocument.FullName.Replace(".h", ".cpp")
        Dim classname = myname.replace(".cpp", "")
        MsgBox(myfullname)
        DTE.ActiveDocument.Selection.StartOfDocument()
        DTE.ActiveDocument.Selection.LineDown(True, 6)
        DTE.ActiveDocument.Selection.Copy()


        DTE.ItemOperations.NewFile("Visual C++\C++ File (.cpp)", myname)
        DTE.ActiveDocument.Selection.Paste()
        DTE.ActiveDocument.Selection.linedown(False)
        DTE.ActiveDocument.Selection.text = "#include """ & oldname & """" & Chr(10)
        DTE.ActiveDocument.Selection.linedown(False)
        DTE.ActiveDocument.Selection.text = "using namespace PhysBAM;" & Chr(10)
        DTE.ActiveDocument.Selection.linedown(False)
        DTE.ActiveDocument.Selection.text = "template class " & classname & "<float>;" & Chr(10)
        DTE.ActiveDocument.Selection.linedown(False)
        DTE.ActiveDocument.Selection.text = "template class " & classname & "<double>;" & Chr(10)
        DTE.ActiveDocument.Save(myfullname)
    End Sub

    Sub CompileSingleFile()
        DTE.ExecuteCommand("Build.Compile")
    End Sub
End Module

